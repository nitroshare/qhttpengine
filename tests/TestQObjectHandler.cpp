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

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTest>
#include <QVariantMap>

#include <QHttpEngine/QHttpSocket>
#include <QHttpEngine/QObjectHandler>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

class DummyAPI : public QObject
{
    Q_OBJECT

public Q_SLOTS:

    int invalidReturnValue() { return 0; }
    QVariantMap invalidArguments(int) { return QVariantMap(); }
    QVariantMap noParameters() { return QVariantMap(); }
    QVariantMap oneParameter(QHttpSocket *) { return QVariantMap(); }
    QVariantMap twoParameters(QHttpSocket *, QVariantMap) { return QVariantMap(); }
    QVariantMap echoPost(QHttpSocket *, QVariantMap d) { return d; }
};

class TestQObjectHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testOldConnection_data();
    void testOldConnection();
    void testNewConnection();
};

void TestQObjectHandler::testOldConnection_data()
{
    QTest::addColumn<bool>("registerPost");
    QTest::addColumn<bool>("requestPost");
    QTest::addColumn<QByteArray>("slot");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QVariantMap>("data");

    QTest::newRow("invalid return")
            << false
            << false
            << QByteArray(SLOT(invalidReturnValue()))
            << static_cast<int>(QHttpSocket::InternalServerError)
            << QVariantMap();

    QTest::newRow("invalid arguments")
            << false
            << false
            << QByteArray(SLOT(invalidArguments(int)))
            << static_cast<int>(QHttpSocket::InternalServerError)
            << QVariantMap();

    QTest::newRow("no parameters")
            << false
            << false
            << QByteArray(SLOT(noParameters()))
            << static_cast<int>(QHttpSocket::OK)
            << QVariantMap();

    QTest::newRow("one parameter")
            << false
            << false
            << QByteArray(SLOT(oneParameter(QHttpSocket*)))
            << static_cast<int>(QHttpSocket::OK)
            << QVariantMap();

    QTest::newRow("two parameters")
            << false
            << false
            << QByteArray(SLOT(twoParameters(QHttpSocket*,QVariantMap)))
            << static_cast<int>(QHttpSocket::OK)
            << QVariantMap();

    QTest::newRow("invalid method")
            << true
            << false
            << QByteArray(SLOT(echoPost(QHttpSocket*,QVariantMap)))
            << static_cast<int>(QHttpSocket::MethodNotAllowed)
            << QVariantMap();

    QTest::newRow("post data")
            << true
            << true
            << QByteArray(SLOT(echoPost(QHttpSocket*,QVariantMap)))
            << static_cast<int>(QHttpSocket::OK)
            << QVariantMap{{"a", "a"}, {"b", 1}};
}

void TestQObjectHandler::testOldConnection()
{
    QFETCH(bool, registerPost);
    QFETCH(bool, requestPost);
    QFETCH(QByteArray, slot);
    QFETCH(int, statusCode);
    QFETCH(QVariantMap, data);

    QObjectHandler handler;
    DummyAPI api;

    handler.registerMethod("test", &api, slot.constData(),
            registerPost ? QHttpSocket::POST : QHttpSocket::GET);

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    if (requestPost) {
        QByteArray buff = QJsonDocument(QJsonObject::fromVariantMap(data)).toJson();
        client.sendHeaders("POST", "test", QHttpSocket::QHttpHeaderMap{
            {"Content-Length", QByteArray::number(buff.length())},
        });
        client.sendData(buff);
    } else {
        client.sendHeaders("GET", "test");
    }

    QTRY_VERIFY(socket.isHeadersParsed());

    handler.route(&socket, socket.path());
    QTRY_COMPARE(client.statusCode(), statusCode);

    if (requestPost) {
        QVERIFY(client.headers().contains("Content-Length"));
        QTRY_COMPARE(client.data().length(), client.headers().value("Content-Length").toInt());
        QCOMPARE(QJsonDocument::fromJson(client.data()).object(), QJsonObject::fromVariantMap(data));
    }
}

void TestQObjectHandler::testNewConnection()
{
    QObjectHandler handler;
    DummyAPI api;

    // Connect to object slot
    handler.registerMethod("0", &api, &DummyAPI::noParameters);
    handler.registerMethod("1", &api, &DummyAPI::oneParameter);
    handler.registerMethod("2", &api, &DummyAPI::twoParameters);

    // Connect to functor
    handler.registerMethod("3", []() { return QVariantMap(); });
    handler.registerMethod("4", &api, []() { return QVariantMap(); });
    handler.registerMethod("5", &api, [](QHttpSocket*) { return QVariantMap(); });
    handler.registerMethod("6", &api, [](QHttpSocket*, QVariantMap d) { return d; });

    for (int i = 0; i < 7; ++i) {
        QSocketPair pair;
        QTRY_VERIFY(pair.isConnected());

        QSimpleHttpClient client(pair.client());
        QHttpSocket socket(pair.server(), &pair);

        client.sendHeaders("GET", QByteArray::number(i));
        QTRY_VERIFY(socket.isHeadersParsed());

        handler.route(&socket, socket.path());
        QTRY_COMPARE(client.statusCode(), static_cast<int>(QHttpSocket::OK));
    }
}

QTEST_MAIN(TestQObjectHandler)
#include "TestQObjectHandler.moc"
