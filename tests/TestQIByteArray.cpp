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

#include <QObject>
#include <QTest>

#include <QHttpEngine/QIByteArray>

const char *Value1 = "test";
const char *Value2 = "TEST";

class TestQIByteArray : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testQString();
    void testQByteArray();
    void testCharPtr();
};

void TestQIByteArray::testQString()
{
    QVERIFY(QIByteArray(Value1) == QString(Value2));
    QVERIFY(QString(Value1) == QIByteArray(Value2));
}

void TestQIByteArray::testQByteArray()
{
    QVERIFY(QIByteArray(Value1) == QByteArray(Value2));
    QVERIFY(QByteArray(Value1) == QIByteArray(Value2));
}

void TestQIByteArray::testCharPtr()
{
    QVERIFY(QIByteArray(Value1) == Value2);
    QVERIFY(Value1 == QIByteArray(Value2));
}

QTEST_MAIN(TestQIByteArray)
#include "TestQIByteArray.moc"
