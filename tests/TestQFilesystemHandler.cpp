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

#include <QDir>
#include <QFile>
#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <QHttpEngine/QHttpSocket>
#include <QHttpEngine/QFilesystemHandler>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

const QByteArray Data = "test";

class TestQFilesystemHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testRequests_data();
    void testRequests();

private:

    bool createFile(const QString &path);
    bool createDirectory(const QString &path);

    QTemporaryDir dir;
};

void TestQFilesystemHandler::initTestCase()
{
    QVERIFY(createFile("outside"));
    QVERIFY(createDirectory("root"));
    QVERIFY(createFile("root/inside"));
}

void TestQFilesystemHandler::testRequests_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("data");

    QTest::newRow("nonexistent resource")
            << "nonexistent"
            << static_cast<int>(QHttpSocket::NotFound)
            << QByteArray();

    QTest::newRow("outside document root")
            << "../outside"
            << static_cast<int>(QHttpSocket::NotFound)
            << QByteArray();

    QTest::newRow("inside document root")
            << "inside"
            << static_cast<int>(QHttpSocket::OK)
            << Data;

    QTest::newRow("directory listing")
            << ""
            << static_cast<int>(QHttpSocket::OK)
            << QByteArray();
}

void TestQFilesystemHandler::testRequests()
{
    QFETCH(QString, path);
    QFETCH(int, statusCode);
    QFETCH(QByteArray, data);

    QFilesystemHandler handler(QDir(dir.path()).absoluteFilePath("root"));

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpSocket socket(pair.server(), &pair);

    handler.route(&socket, path);

    QTRY_COMPARE(client.statusCode(), statusCode);

    if(!data.isNull()) {
        QTRY_COMPARE(client.data(), data);
    }
}

bool TestQFilesystemHandler::createFile(const QString &path)
{
    QFile file(QDir(dir.path()).absoluteFilePath(path));
    if(!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    return file.write(Data) == Data.length();
}

bool TestQFilesystemHandler::createDirectory(const QString &path)
{
    return QDir(dir.path()).mkpath(path);
}

QTEST_MAIN(TestQFilesystemHandler)
#include "TestQFilesystemHandler.moc"
