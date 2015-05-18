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
#include <QPair>
#include <QSignalSpy>
#include <QTest>

#include "common/qsocketpair.h"
#include "qhttpsocket.h"

typedef QPair<QByteArray, QByteArray> Header;
typedef QList<Header> HeaderList;

const QByteArray TestData = "test";

class TestQHttpSocket : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testRequestProperties_data();
    void testRequestProperties();

    void testResponseProperties_data();
    void testResponseProperties();

    void testRead();
    void testWrite();

};

void TestQHttpSocket::initTestCase()
{
    qRegisterMetaType<QHttpSocket::Error>("Error");
}

void TestQHttpSocket::testRequestProperties_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QHttpSocket::Error>("error");
    QTest::addColumn<QByteArray>("method");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<HeaderList>("headers");

    QTest::newRow("simple GET")
            << QByteArray("GET /test HTTP/1.0\r\n\r\n")
            << QHttpSocket::NoError
            << QByteArray("GET")
            << QByteArray("/test")
            << HeaderList();

    QTest::newRow("request header")
            << QByteArray("GET / HTTP/1.0\r\nX-Test: test\r\n\r\n")
            << QHttpSocket::NoError
            << QByteArray("GET")
            << QByteArray("/")
            << (HeaderList() << Header("X-Test", "test"));

    QTest::newRow("malformed request line")
            << QByteArray("test\r\n\r\n")
            << QHttpSocket::MalformedRequestLine;

    QTest::newRow("malformed request header")
            << QByteArray("GET / HTTP/1.0\r\ntest\r\n\r\n")
            << QHttpSocket::MalformedRequestHeader;

    QTest::newRow("invalid HTTP version")
            << QByteArray("GET / HTTP/1.2\r\n\r\n")
            << QHttpSocket::InvalidHttpVersion;
}

void TestQHttpSocket::testRequestProperties()
{
    QFETCH(QByteArray, data);
    QFETCH(QHttpSocket::Error, error);

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QHttpSocket socket(&buffer);

    if(error == QHttpSocket::NoError) {

        QFETCH(QByteArray, method);
        QFETCH(QByteArray, path);
        QFETCH(HeaderList, headers);

        QTRY_VERIFY(socket.headersParsed());

        QCOMPARE(socket.method(), method);
        QCOMPARE(socket.path(), path);
        QCOMPARE(socket.headers().count(), headers.count());
        foreach(Header header, headers) {
           QCOMPARE(socket.header(header.first), header.second);
        }

    } else {

        QSignalSpy errorSpy(&socket, SIGNAL(errorChanged(Error)));

        QTRY_COMPARE(errorSpy.count(), 1);
        QCOMPARE(socket.error(), error);
    }
}

void TestQHttpSocket::testResponseProperties_data()
{
    QTest::addColumn<QByteArray>("response");
    QTest::addColumn<QByteArray>("statusCode");
    QTest::addColumn<HeaderList>("headers");

    QTest::newRow("default values")
            << QByteArray("HTTP/1.0 200 OK\r\n\r\n")
            << QByteArray()
            << HeaderList();

    QTest::newRow("status code")
            << QByteArray("HTTP/1.0 404 FILE NOT FOUND\r\n\r\n")
            << QByteArray("404 FILE NOT FOUND")
            << HeaderList();

    QTest::newRow("response header")
            << QByteArray("HTTP/1.0 200 OK\r\nX-Test: test\r\n\r\n")
            << QByteArray("200 OK")
            << (HeaderList() << Header("X-Test", "test"));
}

void TestQHttpSocket::testResponseProperties()
{
    QFETCH(QByteArray, response);
    QFETCH(QByteArray, statusCode);
    QFETCH(HeaderList, headers);

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);

    QHttpSocket socket(&buffer);

    if(!statusCode.isNull()) {
        socket.setStatusCode(statusCode);
    }

    foreach(Header header, headers) {
        socket.setHeader(header.first, header.second);
    }

    socket.writeHeaders();

    QTRY_COMPARE(data, response);
}

void TestQHttpSocket::testRead()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QHttpSocket socket(pair.server());

    QSignalSpy hPChangedSpy(&socket, SIGNAL(headersParsedChanged(bool)));
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));

    pair.client()->write("POST /test HTTP/1.0\r\n\r\n");

    QTRY_COMPARE(hPChangedSpy.count(), 1);
    QTRY_COMPARE(readyReadSpy.count(), 0);

    pair.client()->write(TestData);

    QTRY_COMPARE(readyReadSpy.count(), 1);

    QCOMPARE(socket.bytesAvailable(), TestData.length());
    QCOMPARE(socket.readAll(), TestData);
}

void TestQHttpSocket::testWrite()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    QHttpSocket socket(&buffer);
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    socket.writeHeaders();
    QCOMPARE(bytesWrittenSpy.count(), 0);

    socket.write(TestData);

    QTRY_COMPARE(bytesWrittenSpy.count(), 1);
    QCOMPARE(bytesWrittenSpy.at(0).at(0).toLongLong(), TestData.length());
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
