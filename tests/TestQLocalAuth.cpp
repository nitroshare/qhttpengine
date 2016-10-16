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

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>
#include <QVariantMap>

#include <QHttpEngine/QHttpSocket>
#include <QHttpEngine/QLocalAuth>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

const QByteArray HeaderName = "X-Test";
const QByteArray CustomName = "Name";
const QByteArray CustomData = "Data";

class TestQLocalAuth : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testAuth();
};

void TestQLocalAuth::testAuth()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    QLocalAuth localAuth;
    localAuth.setData(QVariantMap{
        {CustomName, CustomData}
    });
    localAuth.setHeaderName(HeaderName);
    QVERIFY(localAuth.exists());

    QFile file(localAuth.filename());
    QVERIFY(file.open(QIODevice::ReadOnly));

    QVariantMap data = QJsonDocument::fromJson(file.readAll()).object().toVariantMap();
    QVERIFY(data.contains("token"));
    QCOMPARE(data.value(CustomName).toByteArray(), CustomData);

    client.sendHeaders("GET", "/", QHttpSocket::HeaderMap{
        {HeaderName, data.value("token").toByteArray()}
    });
    QTRY_VERIFY(socket.isHeadersParsed());

    QVERIFY(localAuth.process(&socket));
}

QTEST_MAIN(TestQLocalAuth)
#include "TestQLocalAuth.moc"
