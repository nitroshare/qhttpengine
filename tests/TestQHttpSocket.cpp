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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTest>

#include "qhttpsocket.h"

// Define the data that will be used for the tests
const QString Path = "/test";
const QByteArray HeaderName = "Content-Type";
const QByteArray HeaderValue = "text/plain";
const QByteArray Data = "test";
const int StatusCode = 200;
const QString StatusReason = "OK";

class TestQHttpSocket : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void init();
    void cleanup();

    void testRequestProperties();
    void testRequestData();
    void testResponseProperties();
    void testResponseData();

private:

    QNetworkAccessManager mManager;
    QTcpServer mServer;
    quint16 mPort;

    QNetworkReply *mReply;
    QHttpSocket *mSocket;
};

void TestQHttpSocket::initTestCase()
{
    // Select a random port to listen on
    QVERIFY(mServer.listen());
    mPort = mServer.serverPort();

    mReply = 0;
    mSocket = 0;
}

void TestQHttpSocket::init()
{
    QUrl url(QString("http://127.0.0.1:%2%3").arg(mPort).arg(Path));

    // Create the request and set a raw header
    QNetworkRequest request(url);
    request.setRawHeader(HeaderName, HeaderValue);

    // Send the request, including the POST data
    mReply = mManager.post(request, Data);

    // Ensure that a request was received by the server
    QSignalSpy newConnectionSignalSpy(&mServer, SIGNAL(newConnection()));
    QTRY_COMPARE(newConnectionSignalSpy.count(), 1);

    // Create the HTTP socket
    QTcpSocket *socket = mServer.nextPendingConnection();
    mSocket = new QHttpSocket(socket);
    socket->setParent(mSocket);

    // Ensure that the request headers are received
    QTRY_VERIFY(mSocket->requestHeadersRead());
}

void TestQHttpSocket::cleanup()
{
    if(mReply) {
        mReply->deleteLater();
        mSocket = 0;
    }

    if(mSocket) {
        mSocket->deleteLater();
        mSocket = 0;
    }
}

void TestQHttpSocket::testRequestProperties()
{
    QCOMPARE(mSocket->httpError(), QHttpSocket::NoError);
    QCOMPARE(mSocket->requestMethod(), QString("POST"));
    QCOMPARE(mSocket->requestPath(), Path);
    QCOMPARE(mSocket->requestHeader(HeaderName), QString(HeaderValue));
}

void TestQHttpSocket::testRequestData()
{
    QTRY_COMPARE(mSocket->bytesAvailable(), Data.length());
    QCOMPARE(mSocket->readAll(), Data);
}

void TestQHttpSocket::testResponseProperties()
{
    mSocket->setResponseStatusCode(QString("%1 %2").arg(StatusCode).arg(StatusReason));
    mSocket->setResponseHeader(HeaderName, HeaderValue);
    mSocket->close();

    QTRY_VERIFY(mReply->isFinished());
    QCOMPARE(mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), StatusCode);
    QCOMPARE(mReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString(), StatusReason);
}

void TestQHttpSocket::testResponseData()
{
    mSocket->write(Data);

    QTRY_COMPARE(mReply->bytesAvailable(), Data.length());
    QCOMPARE(mReply->readAll(), Data);
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
