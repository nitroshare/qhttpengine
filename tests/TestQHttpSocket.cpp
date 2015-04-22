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

#include <QListIterator>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTest>

#include "qhttpsocket.h"

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
    void testSignals();
    void testError();

private:

    bool responseHeadersRead();

    QTcpServer mServer;

    QTcpSocket *mClientSocket;
    QHttpSocket *mServerSocket;
};

void TestQHttpSocket::initTestCase()
{
    qRegisterMetaType<QHttpSocket::HttpError>("HttpError");
    QVERIFY(mServer.listen());
}

void TestQHttpSocket::init()
{
    mClientSocket = new QTcpSocket;
    mClientSocket->connectToHost(QHostAddress::LocalHost, mServer.serverPort());
    mServerSocket = 0;

    QTRY_COMPARE(mClientSocket->state(), QAbstractSocket::ConnectedState);

    QTcpSocket *socket = mServer.nextPendingConnection();
    mServerSocket = new QHttpSocket(socket);
    socket->setParent(mServerSocket);
}

void TestQHttpSocket::cleanup()
{
    delete mClientSocket;

    if(mServerSocket) {
        delete mServerSocket;
    }
}

void TestQHttpSocket::testRequestProperties()
{
    mClientSocket->write("PUT /path HTTP/1.1\r\n");
    mClientSocket->write("X-Test: test\r\n\r\n");

    QTRY_VERIFY(mServerSocket->requestHeadersRead());

    QCOMPARE(mServerSocket->httpError(), QHttpSocket::NoError);
    QCOMPARE(mServerSocket->requestMethod(), QByteArray("PUT"));
    QCOMPARE(mServerSocket->requestPath(), QByteArray("/path"));
    QCOMPARE(mServerSocket->requestHeader("X-Test"), QByteArray("test"));
    QVERIFY(mServerSocket->requestHeaders().contains("X-Test"));
}

void TestQHttpSocket::testRequestData()
{
    mClientSocket->write("POST /path HTTP/1.1\r\n\r\n");

    QTRY_VERIFY(mServerSocket->requestHeadersRead());
    QCOMPARE(mServerSocket->bytesAvailable(), 0);

    mClientSocket->write("test");

    QTRY_COMPARE(mServerSocket->bytesAvailable(), 4);
    QCOMPARE(mServerSocket->readAll(), QByteArray("test"));
}

void TestQHttpSocket::testResponseProperties()
{
    mClientSocket->write("GET /path HTTP/1.1\r\n\r\n");
    QTRY_VERIFY(mServerSocket->requestHeadersRead());

    mServerSocket->setResponseStatusCode("403 FORBIDDEN");
    mServerSocket->setResponseHeader("X-Test", "test");
    mServerSocket->flush();

    QTRY_VERIFY(responseHeadersRead());
    QCOMPARE(mClientSocket->readAll(), QByteArray("HTTP/1.0 403 FORBIDDEN\r\nX-Test: test\r\n\r\n"));
}

void TestQHttpSocket::testResponseData()
{
    mClientSocket->write("GET /path HTTP/1.1\r\n\r\n");
    QTRY_VERIFY(mServerSocket->requestHeadersRead());

    mServerSocket->flush();
    QTRY_VERIFY(responseHeadersRead());
    mClientSocket->readAll();

    mServerSocket->write("test");

    QTRY_COMPARE(mClientSocket->bytesAvailable(), 4);
    QCOMPARE(mClientSocket->readAll(), QByteArray("test"));
}

void TestQHttpSocket::testSignals()
{
    QSignalSpy reqHdrReadSpy(mServerSocket, SIGNAL(requestHeadersReadChanged(bool)));
    QSignalSpy readyReadSpy(mServerSocket, SIGNAL(readyRead()));
    QSignalSpy bytesWrittenSpy(mServerSocket, SIGNAL(bytesWritten(qint64)));

    // Ensure that the requestHeadersRead signal was emitted
    mClientSocket->write("GET /path HTTP/1.1\r\n\r\n");
    QTRY_COMPARE(reqHdrReadSpy.count(), 1);

    // Ensure that a single readyRead signal was emitted
    QCOMPARE(readyReadSpy.count(), 0);
    mClientSocket->write("*");
    QTRY_COMPARE(readyReadSpy.count(), 1);

    // Send the response headers and ignore them
    mServerSocket->flush();
    QTRY_VERIFY(responseHeadersRead());
    mClientSocket->readAll();

    // Ensure that a single bytesWritten signal is emitted for each write
    QCOMPARE(bytesWrittenSpy.count(), 0);

    for(int i = 0; i < 2; ++i) {
        mServerSocket->write("*");
        QTRY_COMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).toLongLong(), 1);
    }
}

void TestQHttpSocket::testError()
{
    QSignalSpy errorSpy(mServerSocket, SIGNAL(httpErrorChanged(HttpError)));

    // Send a malformed request line
    mClientSocket->write("error\r\n\r\n");

    QTRY_COMPARE(errorSpy.count(), 1);
    QCOMPARE(mServerSocket->httpError(), QHttpSocket::MalformedRequestLine);
}

bool TestQHttpSocket::responseHeadersRead()
{
    return mClientSocket->peek(65536).indexOf("\r\n\r\n") != -1;
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
