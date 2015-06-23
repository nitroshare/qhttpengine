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
#include <QObject>
#include <QTest>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"
#include "core/qhttpsocket.h"
#include "util/qhttpparser.h"
#include "util/qiodevicecopier.h"

// Utility macro (avoids duplication) that creates a pair of connected
// sockets, a QSimpleHttpClient for the client and a QHttpSocket for the
// server
#define CREATE_SOCKET_PAIR() \
    QSocketPair pair; \
    QTRY_VERIFY(pair.isConnected()); \
    QSimpleHttpClient client(pair.client()); \
    QHttpSocket server(pair.server());

const QByteArray Method = "POST";
const QByteArray Path = "/test";
const int StatusCode = 404;
const QByteArray StatusReason = "NOT FOUND";
const QByteArray Data = "test";

class TestQHttpSocket : public QObject
{
    Q_OBJECT

public:

    TestQHttpSocket();

private Q_SLOTS:

    void testProperties();
    void testData();

private:

    QHttpHeaderMap headers;
};

TestQHttpSocket::TestQHttpSocket()
{
    headers.insert("Content-Type", "text/plain");
    headers.insert("Content-Length", QByteArray::number(Data.length()));
}

void TestQHttpSocket::testProperties()
{
    CREATE_SOCKET_PAIR()

    client.sendHeaders(Method, Path, headers);

    QTRY_COMPARE(server.method(), Method);
    QCOMPARE(server.path(), Path);
    QCOMPARE(server.headers(), headers);

    server.setStatusCode(QByteArray::number(StatusCode) + " " + StatusReason);
    server.setHeaders(headers);
    server.writeHeaders();

    QTRY_COMPARE(client.statusCode(), StatusCode);
    QCOMPARE(client.statusReason(), StatusReason);
    QCOMPARE(client.headers(), headers);
}

void TestQHttpSocket::testData()
{
    CREATE_SOCKET_PAIR()

    client.sendHeaders(Method, Path, headers);
    client.sendData(Data);

    QBuffer buffer;
    QIODeviceCopier copier(&server, &buffer);
    copier.start();

    QTRY_COMPARE(buffer.data(), Data);

    server.writeHeaders();
    server.write(Data);

    QTRY_COMPARE(client.data(), Data);
}

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
