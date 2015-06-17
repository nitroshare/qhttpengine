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

#include "util/qhttpparser.h"
#include "util/qibytearray.h"

typedef QList<QByteArray> QByteArrayList;

Q_DECLARE_METATYPE(QHttpHeaderMap)

const QByteArray Line1 = "a: b";
const QIByteArray Key1 = "a";
const QByteArray Value1 = "b";

const QByteArray Line2 = "c: d";
const QIByteArray Key2 = "c";
const QByteArray Value2 = "d";

class TestQHttpParser : public QObject
{
    Q_OBJECT

public:

    TestQHttpParser();

private Q_SLOTS:

    void testSplit_data();
    void testSplit();

    void testParseHeaderList_data();
    void testParseHeaderList();

    void testParseHeaders_data();
    void testParseHeaders();

private:

    QHttpHeaderMap headers;
};

TestQHttpParser::TestQHttpParser()
{
    headers.insert(Key1, Value1);
    headers.insert(Key2, Value2);
}

void TestQHttpParser::testSplit_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("delim");
    QTest::addColumn<int>("maxSplit");
    QTest::addColumn<QByteArrayList>("parts");

    QTest::newRow("empty string")
            << QByteArray()
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "");

    QTest::newRow("no delimiter")
            << QByteArray("a")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a");

    QTest::newRow("delimiter")
            << QByteArray("a::b::c")
            << QByteArray("::")
            << 0
            << (QByteArrayList() << "a" << "b" << "c");

    QTest::newRow("empty parts")
            << QByteArray("a,,")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a" << "" << "");

    QTest::newRow("maxSplit")
            << QByteArray("a,a,a")
            << QByteArray(",")
            << 1
            << (QByteArrayList() << "a" << "a,a");
}

void TestQHttpParser::testSplit()
{
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, delim);
    QFETCH(int, maxSplit);
    QFETCH(QByteArrayList, parts);

    QByteArrayList outParts;
    QHttpParser::split(data, delim, maxSplit, outParts);

    QCOMPARE(outParts, parts);
}

void TestQHttpParser::testParseHeaderList_data()
{
    QTest::addColumn<QByteArrayList>("lines");
    QTest::addColumn<bool>("success");
    QTest::addColumn<QHttpHeaderMap>("headers");

    QTest::newRow("empty line")
            << (QByteArrayList() << "")
            << false;

    QTest::newRow("multiple lines")
            << (QByteArrayList() << Line1 << Line2)
            << true
            << headers;
}

void TestQHttpParser::testParseHeaderList()
{
    QFETCH(QByteArrayList, lines);
    QFETCH(bool, success);

    QHttpHeaderMap outHeaders;
    QCOMPARE(QHttpParser::parseHeaderList(lines, outHeaders), success);

    if(success) {
        QFETCH(QHttpHeaderMap, headers);
        QCOMPARE(outHeaders, headers);
    }
}

void TestQHttpParser::testParseHeaders_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArrayList>("parts");
    QTest::addColumn<QHttpHeaderMap>("headers");

    QTest::newRow("empty headers")
            << QByteArray("")
            << false;

    QTest::newRow("request")
            << QByteArray("GET / HTTP/1.0\r\n" + Line1 + "\r\n" + Line2)
            << true
            << (QByteArrayList() << "GET" << "/" << "HTTP/1.0")
            << headers;

    QTest::newRow("response")
            << QByteArray("HTTP/1.0 404 NOT FOUND\r\n" + Line1 + "\r\n" + Line2)
            << true
            << (QByteArrayList() << "HTTP/1.0" << "404" << "NOT FOUND")
            << headers;
}

void TestQHttpParser::testParseHeaders()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, success);

    QByteArrayList outParts;
    QHttpHeaderMap outHeaders;

    QCOMPARE(QHttpParser::parseHeaders(data, outParts, outHeaders), success);

    if(success) {
        QFETCH(QByteArrayList, parts);
        QFETCH(QHttpHeaderMap, headers);

        QCOMPARE(outParts, parts);
        QCOMPARE(outHeaders, headers);
    }
}

QTEST_MAIN(TestQHttpParser)
#include "TestQHttpParser.moc"
