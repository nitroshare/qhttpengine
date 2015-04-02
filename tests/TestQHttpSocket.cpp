/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTest>

#include "qhttpsocket.h"

class TestQHttpSocket : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void testMethods();

private:

    QNetworkAccessManager mManager;
    QTcpServer mServer;
    quint16 mPort;
};

void TestQHttpSocket::initTestCase()
{
    QVERIFY(mServer.listen(QHostAddress("127.0.0.1")));
    mPort = mServer.serverPort();
}

void TestQHttpSocket::testMethods()
{
    // Issue a simple request to the server
    QNetworkRequest request(QUrl(QString("http://127.0.0.1:%2/test").arg(mPort)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    QNetworkReply *reply = mManager.post(request, "test");

    // Ensure that a request was received by the server
    QSignalSpy newSignalSpy(&mServer, SIGNAL(newConnection()));
    QTRY_COMPARE(newSignalSpy.count(), 1);

    // Create the HTTP socket
    QHttpSocket *socket = new QHttpSocket(mServer.nextPendingConnection());

    // Create spies for the signals emitted by the socket
    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    // Verify the request headers
    QTRY_VERIFY(socket->requestHeadersRead());
    QCOMPARE(socket->error(), QHttpSocket::None);
    QCOMPARE(socket->requestMethod(), QString("POST"));
    QCOMPARE(socket->requestUri(), QString("/test"));
    QCOMPARE(socket->requestHeader("Content-Type"), QString("text/plain"));

    // Verify the request data
    QTRY_COMPARE(readyReadSpy.count(), 1);
    QCOMPARE(socket->readAll(), QByteArray("test"));

    // Write response data and close the socket
    socket->setResponseStatusCode("200 OK");
    socket->setResponseHeader("Content-Type", "text/plain");
    socket->write("test");
    socket->close();

    // Wait for the indication that the correct number of bytes were written
    QTRY_COMPARE(bytesWrittenSpy.count(), 1);
    QCOMPARE(bytesWrittenSpy.at(0).at(0).toInt(), 4);

    // Wait for the reply to indicate the request finished
    QTRY_VERIFY(reply->isFinished());

    // Examine the data from the reply
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute), QVariant(200));
    QCOMPARE(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute), QVariant("OK"));
    QCOMPARE(reply->header(QNetworkRequest::ContentTypeHeader), QVariant("text/plain"));
    QCOMPARE(reply->readAll(), QByteArray("test"));

    socket->deleteLater();
    reply->deleteLater();
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
