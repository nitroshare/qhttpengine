/*
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

#include <QTest>

#include <QHttpEngine/QHttpHandler>
#include <QHttpEngine/QHttpMiddleware>
#include <QHttpEngine/QHttpSocket>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

class DummyMiddleware : public QHttpMiddleware
{
    Q_OBJECT

public:

    virtual bool process(QHttpSocket *socket)
    {
        socket->writeError(QHttpSocket::Forbidden);
        return false;
    }
};

class TestQHttpMiddleware : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testProcess();
};

void TestQHttpMiddleware::testProcess()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    client.sendHeaders("GET", "/");
    QTRY_VERIFY(socket.isHeadersParsed());

    DummyMiddleware middleware;
    QHttpHandler handler;
    handler.addMiddleware(&middleware);
    handler.route(&socket, "/");

    QTRY_COMPARE(client.statusCode(), static_cast<int>(QHttpSocket::Forbidden));
}

QTEST_MAIN(TestQHttpMiddleware)
#include "TestQHttpMiddleware.moc"
