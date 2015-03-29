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

// Allow qintptr to be used in a signal
Q_DECLARE_METATYPE(qintptr)

class TcpServer : public QTcpServer
{
    Q_OBJECT

Q_SIGNALS:

    void newSocketDescriptor(qintptr socketDescriptor);

private:

    virtual void incomingConnection(qintptr socketDescriptor) {
        Q_EMIT newSocketDescriptor(socketDescriptor);
    }
};

class TestQHttpSocket : public QObject
{
    Q_OBJECT

public:

    TestQHttpSocket();

private Q_SLOTS:

    // Initialization and cleanup
    void init();
    void cleanup();

    // Tests for each of the methods
    void testRequestData();
    void testResponseData();

    // Tests for predefined conditions
    void testErrors();

private:

    TcpServer mServer;
    quint16 mPort;

    QTcpSocket *mClient;
    QHttpSocket *mSocket;
};

TestQHttpSocket::TestQHttpSocket()
    : mClient(0),
      mSocket(0)
{
    mServer.listen(QHostAddress::LocalHost);
    mPort = mServer.serverPort();
}

void TestQHttpSocket::init()
{
    QSignalSpy spy(&mServer, SIGNAL(newSocketDescriptor(qintptr)));

    mClient = new QTcpSocket;
    mClient->connectToHost(QHostAddress(QHostAddress::LocalHost), mPort);

    // Wait for the client and socket to establish a connection
    QTRY_COMPARE(spy.count(), 1);
    QTRY_VERIFY(mClient->isOpen());

    mSocket = new QHttpSocket(spy.at(0).at(0).value<qintptr>());
}

void TestQHttpSocket::cleanup()
{
    if(mClient) {
        mClient->deleteLater();
        mClient = 0;
    }

    if(mSocket) {
        mSocket->deleteLater();
        mSocket = 0;
    }
}

void TestQHttpSocket::testRequestData()
{
    QSignalSpy spy(mSocket, SIGNAL(requestHeadersParsed()));

    // Have the client send an empty request
    mClient->write(
        "GET /test HTTP/1.0\r\n"
        "Content-type: text/html\r\n"
        "Content-length: 0\r\n"
        "\r\n"
    );

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(mSocket->error(), QHttpSocket::None);
    QCOMPARE(mSocket->requestMethod(), QString("GET"));
    QCOMPARE(mSocket->requestUri(), QString("/test"));
    QCOMPARE(mSocket->requestHeaders().count(), 2);
    QCOMPARE(mSocket->requestHeader("content-type"), QString("text/html"));
    QCOMPARE(mSocket->requestHeader("content-length"), QString("0"));
}

void TestQHttpSocket::testResponseData()
{
    //...
}

void TestQHttpSocket::testErrors()
{
    //...
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
