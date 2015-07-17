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

class DummyHandler : public QObjectHandler
{
    Q_OBJECT

private Q_SLOTS:

    void invalidSignature(QVariantMap) {}
    QVariantMap validSlot(QVariantMap params) {
        return params;
    }
};

class TestQObjectHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testRequests_data();
    void testRequests();
};

void TestQObjectHandler::testRequests_data()
{
    QTest::addColumn<QByteArray>("method");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("statusCode");

    QVariantMap map;
    map.insert("param1", 1);
    map.insert("param2", 2);

    QByteArray data = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson();

    QTest::newRow("nonexistent slot")
            << QByteArray("POST")
            << QByteArray("nonexistent")
            << data
            << static_cast<int>(QHttpSocket::NotFound);

    QTest::newRow("invalid signature")
            << QByteArray("POST")
            << QByteArray("invalidSignature")
            << data
            << static_cast<int>(QHttpSocket::InternalServerError);

    QTest::newRow("bad method")
            << QByteArray("GET")
            << QByteArray("validSlot")
            << data
            << static_cast<int>(QHttpSocket::MethodNotAllowed);

    QTest::newRow("malformed JSON")
            << QByteArray("POST")
            << QByteArray("validSlot")
            << QByteArray("")
            << static_cast<int>(QHttpSocket::BadRequest);

    QTest::newRow("valid slot")
            << QByteArray("POST")
            << QByteArray("validSlot")
            << data
            << static_cast<int>(QHttpSocket::OK);
}

void TestQObjectHandler::testRequests()
{
    QFETCH(QByteArray, method);
    QFETCH(QByteArray, path);
    QFETCH(QByteArray, data);
    QFETCH(int, statusCode);

    DummyHandler handler;

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    QHttpHeaderMap headers;
    headers.insert("Content-Length", QByteArray::number(data.length()));

    client.sendHeaders(method, path, headers);
    client.sendData(data);

    QTRY_VERIFY(socket.isHeadersParsed());

    handler.route(&socket, socket.path());

    QTRY_COMPARE(client.statusCode(), statusCode);

    if(statusCode == QHttpSocket::OK) {
        QVERIFY(client.headers().contains("Content-Length"));
        QTRY_COMPARE(client.data().length(), client.headers().value("Content-Length").toInt());
        QCOMPARE(QJsonDocument::fromJson(client.data()), QJsonDocument::fromJson(data));
    }
}

QTEST_MAIN(TestQObjectHandler)
#include "TestQObjectHandler.moc"
