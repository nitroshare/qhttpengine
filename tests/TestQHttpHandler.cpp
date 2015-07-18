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

#include <QRegExp>
#include <QTest>

#include <QHttpEngine/QHttpSocket>
#include <QHttpEngine/QHttpHandler>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

class DummyHandler : public QHttpHandler
{
    Q_OBJECT

public:

    virtual void process(QHttpSocket *socket, const QString &path) {
        mPathRemainder = path;
        socket->writeHeaders();
        socket->close();
    }

    QString mPathRemainder;
};

class TestQHttpHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testRedirect_data();
    void testRedirect();

    void testSubHandler_data();
    void testSubHandler();
};

void TestQHttpHandler::testRedirect_data()
{
    QTest::addColumn<QRegExp>("pattern");
    QTest::addColumn<QString>("destination");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("location");

    QTest::newRow("match")
            << QRegExp("\\w+")
            << QString("/two")
            << QByteArray("one")
            << static_cast<int>(QHttpSocket::Found)
            << QByteArray("/two");

    QTest::newRow("no match")
            << QRegExp("\\d+")
            << QString("")
            << QByteArray("test")
            << static_cast<int>(QHttpSocket::NotFound);

    QTest::newRow("captured texts")
            << QRegExp("(\\d+)")
            << QString("/path/%1")
            << QByteArray("123")
            << static_cast<int>(QHttpSocket::Found)
            << QByteArray("/path/123");
}

void TestQHttpHandler::testRedirect()
{
    QFETCH(QRegExp, pattern);
    QFETCH(QString, destination);
    QFETCH(QByteArray, path);
    QFETCH(int, statusCode);

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    client.sendHeaders("GET", path, QHttpHeaderMap());
    QTRY_VERIFY(socket.isHeadersParsed());

    QHttpHandler handler;
    handler.addRedirect(pattern, destination);
    handler.route(&socket, socket.path());

    QTRY_COMPARE(client.statusCode(), statusCode);

    if(statusCode == QHttpSocket::Found) {
        QFETCH(QByteArray, location);
        QCOMPARE(client.headers().value("Location"), location);
    }
}

void TestQHttpHandler::testSubHandler_data()
{
    QTest::addColumn<QRegExp>("pattern");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<QString>("pathRemainder");
    QTest::addColumn<int>("statusCode");

    QTest::newRow("match")
            << QRegExp("\\w+")
            << QByteArray("test")
            << QString("")
            << static_cast<int>(QHttpSocket::OK);

    QTest::newRow("no match")
            << QRegExp("\\d+")
            << QByteArray("test")
            << QString("")
            << static_cast<int>(QHttpSocket::NotFound);

    QTest::newRow("path")
            << QRegExp("one/")
            << QByteArray("one/two")
            << QString("two")
            << static_cast<int>(QHttpSocket::OK);
}

void TestQHttpHandler::testSubHandler()
{
    QFETCH(QRegExp, pattern);
    QFETCH(QByteArray, path);
    QFETCH(QString, pathRemainder);
    QFETCH(int, statusCode);

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    client.sendHeaders("GET", path, QHttpHeaderMap());
    QTRY_VERIFY(socket.isHeadersParsed());

    DummyHandler subHandler;
    QHttpHandler handler;
    handler.addSubHandler(pattern, &subHandler);

    handler.route(&socket, socket.path());

    QTRY_COMPARE(client.statusCode(), statusCode);
    QCOMPARE(subHandler.mPathRemainder, pathRemainder);
}

QTEST_MAIN(TestQHttpHandler)
#include "TestQHttpHandler.moc"
