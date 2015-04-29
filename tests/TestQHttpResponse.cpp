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

#include <QBuffer>
#include <QList>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "qhttpresponse.h"

class TestQHttpResponse : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testProperties();
    void testWrite();

private:

    QList<QByteArray> split(const QByteArray &data, const QByteArray &delim);
};

void TestQHttpResponse::testProperties()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);

    QHttpResponse response(&buffer);
    response.setStatusCode("404 NOT FOUND");
    response.setHeader("X-Test", "test");
    response.writeHeaders();

    QCOMPARE(data.right(4), QByteArray("\r\n\r\n"));
    data.chop(4);

    QList<QByteArray> lines = split(data, "\r\n");
    QCOMPARE(lines.takeFirst(), QByteArray("HTTP/1.0 404 NOT FOUND"));
    QVERIFY(lines.contains("X-Test: test"));
}

void TestQHttpResponse::testWrite()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);

    QHttpResponse response(&buffer);
    QSignalSpy bytesWrittenSpy(&response, SIGNAL(bytesWritten(qint64)));

    response.writeHeaders();
    QCOMPARE(bytesWrittenSpy.count(), 0);

    response.write("*");

    QTRY_COMPARE(bytesWrittenSpy.count(), 1);
    QCOMPARE(bytesWrittenSpy.at(0).at(0).toLongLong(), 1);
}

// TODO: move this into a separate class / namespace since it is essentially
// copied-and-pasted unmodified from the QHttpResponse class

QList<QByteArray> TestQHttpResponse::split(const QByteArray &data, const QByteArray &delim)
{
    QList<QByteArray> parts;
    int index = 0;

    forever {
        int nextIndex = data.indexOf(delim, index);

        // If the delimiter wasn't found, the final part is the remainder of the string
        if(nextIndex == -1) {
            parts.append(data.mid(index));
            break;
        }

        parts.append(data.mid(index, nextIndex - index));
        index = nextIndex + delim.length();
    }

    return parts;
}

QTEST_MAIN(TestQHttpResponse)
#include "TestQHttpResponse.moc"
