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

#include "common/qiodevicecounter.h"
#include "common/qsocketpair.h"
#include "qhttpengine.h"
#include "qhttpsocket.h"
#include "qiodevicecopier.h"

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
    QTest::addColumn<QByteArray>("statusLine");
    QTest::addColumn<QByteArray>("statusCode");
    QTest::addColumn<HeaderList>("headers");
    QTest::addColumn<QByteArray>("data");

    QTest::newRow("default values")
            << QByteArray("HTTP/1.0 200 OK")
            << QByteArray()
            << HeaderList()
            << QByteArray();

    QTest::newRow("status code")
            << QByteArray("HTTP/1.0 404 FILE NOT FOUND")
            << QByteArray("404 FILE NOT FOUND")
            << HeaderList()
            << QByteArray();

    QTest::newRow("response header")
            << QByteArray("HTTP/1.0 200 OK")
            << QByteArray()
            << (HeaderList() << Header("X-Test1", "test") << Header("X-Test2", "test"))
            << QByteArray();

    QTest::newRow("data")
            << QByteArray("HTTP/1.0 200 OK")
            << QByteArray()
            << HeaderList()
            << TestData;
}

void TestQHttpSocket::testResponseProperties()
{
    QFETCH(QByteArray, statusLine);
    QFETCH(QByteArray, statusCode);
    QFETCH(HeaderList, headers);
    QFETCH(QByteArray, data);

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());
    QHttpSocket socket(pair.server());

    if(!statusCode.isNull()) {
        socket.setStatusCode(statusCode);
    }

    foreach(Header header, headers) {
        socket.setHeader(header.first, header.second);
    }

    // Create a buffer for storing data received by the HTTP client
    QByteArray bufferData;
    QBuffer buffer(&bufferData);
    buffer.open(QIODevice::WriteOnly);

    // Copy data from the client into the buffer as it becomes available
    QIODeviceCopier copier(pair.client(), &buffer);
    copier.start();

    // Write the headers and wait until they are received by the client
    socket.writeHeaders();
    QTRY_VERIFY(bufferData.indexOf("\r\n\r\n") != -1);

    // If data was provided, write it and read it back from the client
    if(!data.isNull()) {

        // Store the length of the headers
        qint64 headerLength = bufferData.length();

        // Write the data to the socket, counting the amount written
        QIODeviceCounter counter(&socket);
        socket.write(data);

        // Verify that the expected amount was written and read
        QTRY_COMPARE(counter.bytesWritten(), data.length());
        QTRY_COMPARE(bufferData.length(),  headerLength + data.length());
    }

    // Split the response into fragments and compare the expected values
    QList<QByteArray> parts = QHttpEngine::split(bufferData, "\r\n\r\n", 1);
    QList<QByteArray> lines = QHttpEngine::split(parts.at(0), "\r\n");

    QCOMPARE(lines.takeFirst(), statusLine);
    QCOMPARE(lines.count(), headers.count());

    foreach(Header header, headers) {
        QVERIFY(lines.contains(header.first + ": " + header.second));
    }

    QCOMPARE(parts.at(1), data);
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
