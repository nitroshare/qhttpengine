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

#include <QObject>
#include <QTest>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"
#include "core/qhttpsocket.h"
#include "util/qhttpparser.h"

const QByteArray Method = "GET";
const QByteArray Path = "/test";
const int StatusCode = 404;
const QByteArray StatusReason = "NOT FOUND";

class TestQHttpSocket : public QObject
{
    Q_OBJECT

public:

    TestQHttpSocket();

private Q_SLOTS:

    void testProperties();

private:

    QHttpHeaderMap headers;
};

TestQHttpSocket::TestQHttpSocket()
{
    headers.insert("a", "b");
    headers.insert("c", "d");
}

void TestQHttpSocket::testProperties()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket server(pair.server());

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

QTEST_MAIN(TestQHttpSocket)
#include "TestQHttpSocket.moc"
