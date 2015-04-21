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

private:

    QTcpServer mServer;

    QTcpSocket *mClientSocket;
    QHttpSocket *mServerSocket;
};

void TestQHttpSocket::initTestCase()
{
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
    QCOMPARE(mServerSocket->requestMethod(), QString("PUT"));
    QCOMPARE(mServerSocket->requestPath(), QString("/path"));
    QCOMPARE(mServerSocket->requestHeader("X-Test"), QString("test"));
}

void TestQHttpSocket::testRequestData()
{
    mClientSocket->write("POST /path HTTP/1.1\r\n\r\n");

    QTRY_VERIFY(mServerSocket->requestHeadersRead());
    QCOMPARE(mServerSocket->bytesAvailable(), 0);

    QByteArray data(65536, '*');
    mClientSocket->write(data);

    QTRY_COMPARE(mServerSocket->bytesAvailable(), data.length());
    QCOMPARE(mServerSocket->readAll(), data);
}

void TestQHttpSocket::testResponseProperties()
{
    mClientSocket->write("GET /path HTTP/1.1\r\n\r\n");
    QTRY_VERIFY(mServerSocket->requestHeadersRead());

    mServerSocket->setResponseStatusCode("403 FORBIDDEN");
    mServerSocket->setResponseHeader("X-Test", "test");
    mServerSocket->flush();

    QTRY_VERIFY(mClientSocket->peek(65536).indexOf("\r\n\r\n") != -1);
    QCOMPARE(mClientSocket->readAll(), QByteArray("HTTP/1.0 403 FORBIDDEN\r\nX-Test: test\r\n\r\n"));
}

void TestQHttpSocket::testResponseData()
{
    mClientSocket->write("GET /path HTTP/1.1\r\n\r\n");
    QTRY_VERIFY(mServerSocket->requestHeadersRead());

    mServerSocket->flush();
    QTRY_VERIFY(mClientSocket->peek(65536).indexOf("\r\n\r\n") != -1);
    mClientSocket->read(mClientSocket->peek(65536).indexOf("\r\n\r\n") + 4);

    QByteArray data(65536, '*');
    mServerSocket->write(data);

    QTRY_COMPARE(mClientSocket->bytesAvailable(), data.length());
    QCOMPARE(mClientSocket->readAll(), data);
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
