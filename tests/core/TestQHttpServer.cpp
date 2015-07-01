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

#include <QTcpSocket>
#include <QTest>

#include "common/qsimplehttpclient.h"
#include "core/qhttpserver.h"
#include "handler/qhttphandler.h"

class DummyHandler : public QHttpHandler
{
    Q_OBJECT

public:

    DummyHandler() : mSocket(0) {}

    virtual bool process(QHttpSocket *socket, const QString &path) {
        mSocket = socket;
        mPath = path;
        return true;
    }

    QHttpSocket *mSocket;
    QString mPath;
};

class TestQHttpServer : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testServer();
};

void TestQHttpServer::testServer()
{
    DummyHandler handler;
    QHttpServer server(&handler);

    QVERIFY(server.listen(QHostAddress::LocalHost));

    QTcpSocket socket;
    socket.connectToHost(server.address(), server.port());
    QTRY_VERIFY(socket.isValid());

    QSimpleHttpClient client(&socket);
    client.sendHeaders("GET", "/test", QHttpHeaderMap());

    QTRY_VERIFY(handler.mSocket != 0);
    QCOMPARE(handler.mPath, QString("test"));
}

QTEST_MAIN(TestQHttpServer)
#include "TestQHttpServer.moc"
