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
 */

#include <QBuffer>
#include <QList>
#include <QObject>
#include <QPair>
#include <QSignalSpy>
#include <QTest>
#include <QTcpServer>
#include <QTcpSocket>

#include "qhttprequest.h"

typedef QPair<QByteArray, QByteArray> Header;
typedef QList<Header> HeaderList;

class TestQHttpRequest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testProperties_data();
    void testProperties();

    void testRead();
};

void TestQHttpRequest::initTestCase()
{
    qRegisterMetaType<QHttpRequest::Error>("Error");
}

void TestQHttpRequest::testProperties_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QHttpRequest::Error>("error");
    QTest::addColumn<QByteArray>("method");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<HeaderList>("headers");

    QTest::newRow("simple GET")
            << QByteArray("GET /test HTTP/1.0\r\n\r\n")
            << QHttpRequest::NoError
            << QByteArray("GET")
            << QByteArray("/test")
            << HeaderList();

    QTest::newRow("request header")
            << QByteArray("GET / HTTP/1.0\r\nX-Test: test\r\n\r\n")
            << QHttpRequest::NoError
            << QByteArray("GET")
            << QByteArray("/")
            << (HeaderList() << Header("X-Test", "test"));

    QTest::newRow("malformed request line")
            << QByteArray("test\r\n\r\n")
            << QHttpRequest::MalformedRequestLine;

    QTest::newRow("malformed request header")
            << QByteArray("GET / HTTP/1.0\r\ntest\r\n\r\n")
            << QHttpRequest::MalformedRequestHeader;

    QTest::newRow("invalid HTTP version")
            << QByteArray("GET / HTTP/1.2\r\n\r\n")
            << QHttpRequest::InvalidHttpVersion;
}

void TestQHttpRequest::testProperties()
{
    QFETCH(QByteArray, data);
    QFETCH(QHttpRequest::Error, error);

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QHttpRequest request(&buffer);

    if(error == QHttpRequest::NoError) {

        QFETCH(QByteArray, method);
        QFETCH(QByteArray, path);
        QFETCH(HeaderList, headers);

        QTRY_VERIFY(request.headersParsed());

        QCOMPARE(request.method(), method);
        QCOMPARE(request.path(), path);
        QCOMPARE(request.headers().count(), headers.count());
        foreach(Header header, headers) {
           QCOMPARE(request.header(header.first), header.second);
        }

    } else {

        QSignalSpy errorSpy(&request, SIGNAL(errorChanged(Error)));

        QTRY_COMPARE(errorSpy.count(), 1);
        QCOMPARE(request.error(), error);
    }
}

void TestQHttpRequest::testRead()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost));

    QTcpSocket clientSocket;
    clientSocket.connectToHost(server.serverAddress(), server.serverPort());

    QTRY_COMPARE(clientSocket.state(), QAbstractSocket::ConnectedState);

    QTcpSocket *serverSocket = server.nextPendingConnection();
    QHttpRequest request(serverSocket);

    QSignalSpy hPChangedSpy(&request, SIGNAL(headersParsedChanged(bool)));
    QSignalSpy readyReadSpy(&request, SIGNAL(readyRead()));

    clientSocket.write("POST /test HTTP/1.0\r\n\r\n");

    QTRY_COMPARE(hPChangedSpy.count(), 1);
    QTRY_COMPARE(readyReadSpy.count(), 0);

    clientSocket.write("*");

    QTRY_COMPARE(readyReadSpy.count(), 1);

    QCOMPARE(request.bytesAvailable(), 1);
    QCOMPARE(request.readAll(), QByteArray("*"));
}

QTEST_MAIN(TestQHttpRequest)
#include "TestQHttpRequest.moc"
