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

#include <QList>
#include <QObject>
#include <QTest>

#include <qhttpengine.h>

typedef QList<QByteArray> QByteArrayList;

class TestQHttpEngine : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testSplit_data();
    void testSplit();
};

void TestQHttpEngine::testSplit_data()
{
    QTest::addColumn<QByteArray>("original");
    QTest::addColumn<QByteArray>("delimiter");
    QTest::addColumn<QByteArrayList>("list");

    QTest::newRow("empty string")
            << QByteArray()
            << QByteArray(",")
            << (QByteArrayList() << "");

    QTest::newRow("no delimiter")
            << QByteArray("a")
            << QByteArray(",")
            << (QByteArrayList() << "a");

    QTest::newRow("single-char delimiter")
            << QByteArray("a,b,c")
            << QByteArray(",")
            << (QByteArrayList() << "a" << "b" << "c");

    QTest::newRow("multi-char delimiter")
            << QByteArray("a::b::c")
            << QByteArray("::")
            << (QByteArrayList() << "a" << "b" << "c");

    QTest::newRow("empty parts")
            << QByteArray("a,,")
            << QByteArray(",")
            << (QByteArrayList() << "a" << "" << "");
}

void TestQHttpEngine::testSplit()
{
    QFETCH(QByteArray, original);
    QFETCH(QByteArray, delimiter);
    QFETCH(QByteArrayList, list);

    QCOMPARE(QHttpEngine::split(original, delimiter), list);
}

QTEST_MAIN(TestQHttpEngine)
#include "TestQHttpEngine.moc"
