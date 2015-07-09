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

#include <QHttpSocket>
#include <QSubHandler>

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

class TestQSubHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testPatterns_data();
    void testPatterns();
};

void TestQSubHandler::testPatterns_data()
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

void TestQSubHandler::testPatterns()
{
    QFETCH(QRegExp, pattern);
    QFETCH(QByteArray, path);
    QFETCH(QString, pathRemainder);
    QFETCH(int, statusCode);

    DummyHandler handler;

    QSubHandler subHandler;
    subHandler.addHandler(pattern, &handler);

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket *socket = new QHttpSocket(pair.server(), &pair);

    client.sendHeaders("GET", path, QHttpHeaderMap());
    QTRY_VERIFY(socket->isHeadersParsed());

    subHandler.process(socket, socket->path());

    QTRY_COMPARE(client.statusCode(), statusCode);
    QCOMPARE(handler.mPathRemainder, pathRemainder);
}

QTEST_MAIN(TestQSubHandler)
#include "TestQSubHandler.moc"
