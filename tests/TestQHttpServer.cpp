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

#include <QSignalSpy>
#include <QTcpSocket>
#include <QTest>

#if !defined(QT_NO_SSL)
#  include <QFile>
#  include <QSslCertificate>
#  include <QSslConfiguration>
#  include <QSslKey>
#  include <QSslSocket>
#endif

#include <qhttpengine/qhttpserver.h>
#include <qhttpengine/qhttphandler.h>

#include "common/qsimplehttpclient.h"

class TestHandler : public QHttpHandler
{
    Q_OBJECT

public:

    TestHandler() : mSocket(0) {}

    virtual void process(QHttpSocket *socket, const QString &path) {
        mSocket = socket;
        mPath = path;
    }

    QHttpSocket *mSocket;
    QString mPath;
};

class TestQHttpServer : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testServer();

#if !defined(QT_NO_SSL)
    void testSsl();
#endif
};

void TestQHttpServer::testServer()
{
    TestHandler handler;
    QHttpServer server(&handler);

    QVERIFY(server.listen(QHostAddress::LocalHost));

    QTcpSocket socket;
    socket.connectToHost(server.serverAddress(), server.serverPort());
    QTRY_COMPARE(socket.state(), QAbstractSocket::ConnectedState);

    QSimpleHttpClient client(&socket);
    client.sendHeaders("GET", "/test");

    QTRY_VERIFY(handler.mSocket != 0);
    QCOMPARE(handler.mPath, QString("test"));

    QSignalSpy destroyedSpy(handler.mSocket, SIGNAL(destroyed()));
    handler.mSocket->close();
    QTRY_COMPARE(destroyedSpy.count(), 1);
}

#if !defined(QT_NO_SSL)
void TestQHttpServer::testSsl()
{
    QFile keyFile(":/key.pem");
    QVERIFY(keyFile.open(QIODevice::ReadOnly));

    QSslKey key(&keyFile, QSsl::Rsa);
    QList<QSslCertificate> certs = QSslCertificate::fromPath(":/cert.pem");

    QSslConfiguration config;
    config.setPrivateKey(key);
    config.setLocalCertificateChain(certs);

    QHttpServer server;
    server.setSslConfiguration(config);

    QVERIFY(server.listen(QHostAddress::LocalHost));

    QSslSocket socket;
    socket.setCaCertificates(certs);
    socket.connectToHost(server.serverAddress(), server.serverPort());
    socket.setPeerVerifyName("localhost");

    QTRY_COMPARE(socket.state(), QAbstractSocket::ConnectedState);

    socket.startClientEncryption();
    QTRY_VERIFY(socket.isEncrypted());
}
#endif

QTEST_MAIN(TestQHttpServer)
#include "TestQHttpServer.moc"
