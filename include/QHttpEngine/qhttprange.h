/*
 * Copyright (c) 2016 Aleksei Ermakov
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

#ifndef QHTTPENGINE_QHTTPRANGE_H
#define QHTTPENGINE_QHTTPRANGE_H

#include <QString>

#include "qhttpengine_global.h"

class QHTTPENGINE_EXPORT QHttpRangePrivate;

/**
 * @brief HTTP range representation
 * @headerfile qhttprange.h QHttpEngine/QHttpRange
 *
 * This class provides a representation of HTTP range, described in RFC 7233
 * and used when partial content is requested by the client. When an object is
 * created, optional dataSize can be specified, so that relative ranges can
 * be represented as absolute.
 *
 * Example:
 * @code
 * QHttpRange range(10, -1, 90);
 * range.from();   //  10
 * range.to();     //  89
 * range.length(); //  80
 *
 * range = QHttpRange("-500", 1000);
 * range.from();   // 500
 * range.to();     // 999
 * range.length(); // 500
 *
 * range = QHttpRange(0, -1);
 * range.from();   //   0
 * range.to();     //  -1
 * range.length(); //  -1
 *
 * range = QHttpRange(range, 100);
 * range.from();   //   0
 * range.to();     //  99
 * range.length(); // 100
 * @endcode
 *
 */
class QHTTPENGINE_EXPORT QHttpRange
{
public:

    /**
     * @brief Default QHttpRange constructor
     *
     * An empty QHttpRange is considered invalid.
     */
    QHttpRange();

    /**
     * @brief Construct QHttpRange by parsing range
     *
     * Parses string representation range and constructs new QHttpRange.
     * For raw header "Range: bytes=0-100" only "0-100" should be passed to
     * constructor. dataSize may be supplied so that relative ranges could be
     * represented as absolute values.
     */
    QHttpRange(const QString &range, qint64 dataSize = -1);

    /**
     * @brief Construct QHttpRange, using from and to values
     *
     * Initialises a new QHttpRange with from and to values. dataSize may be
     * supplied so that relative ranges could be represented as
     * absolute values.
     */
    QHttpRange(qint64 from, qint64 to, qint64 dataSize = -1);

    /**
     * @brief Construct QHttpRange from other QHttpRange and dataSize
     *
     * Initialises a new QHttpRange with from and to values of other
     * QHttpRequest. Supplied dataSize is used instead of other dataSize.
     */
    QHttpRange(const QHttpRange &other, qint64 dataSize);

    /**
     * @brief Destroy the range
     */
    ~QHttpRange();

    /**
     * @brief Assignment operator
     */
    QHttpRange& operator=(const QHttpRange &other);

    /**
     * @brief Return starting position of range
     *
     * If range is set as 'last N bytes' and dataSize is not set, returns -N.
     *
     * Example:
     * @code
     * QHttpRange range("-500");
     * range.from();   // -500
     * range.to();     //   -1
     * range.length(); //  500
     *
     * range = QHttpRange(range, 800);
     * range.from();   //  300
     * range.to();     //  799
     * range.length(); //  500
     *
     * range = QHttpRange("10-");
     * range.from();   //   10
     * range.to();     //   -1
     * range.length(); //   -1
     *
     * range = QHttpRange(range, 100);
     * range.from();   //   10
     * range.to();     //   99
     * range.length(); //   90
     * @endcode
     *
     */
    qint64 from() const;

    /**
     * @brief Returns ending position of range.
     *
     * If range is set as 'last N bytes' and dataSize is not set, returns -1.
     * If ending position is not set, and dataSize is not set, returns -1.
     *
     * Example:
     * @code
     * QHttpRange range("-500");
     * range.from();   // -500
     * range.to();     //   -1
     * range.length(); //  500
     *
     * range = QHttpRange(range, 800);
     * range.from();   //  300
     * range.to();     //  799
     * range.length(); //  500
     *
     * range = QHttpRange("10-");
     * range.from();   //   10
     * range.to();     //   -1
     * range.length(); //   -1
     *
     * range = QHttpRange(range, 100);
     * range.from();   //   10
     * range.to();     //   99
     * range.length(); //   90
     * @endcode
     *
     */
    qint64 to() const;

    /**
     * @brief Returns length of range.
     *
     * If ending position is not set, and dataSize is not set, and range is
     * not set as 'last N bytes', returns -1. If range is invalid, returns -1.
     *
     * Example:
     * @code
     * QHttpRange range("-500");
     * range.from();   // -500
     * range.to();     //   -1
     * range.length(); //  500
     *
     * range = QHttpRange(range, 800);
     * range.from();   //  300
     * range.to();     //  799
     * range.length(); //  500
     *
     * range = QHttpRange("10-");
     * range.from();   //   10
     * range.to();     //   -1
     * range.length(); //   -1
     *
     * range = QHttpRange(range, 100);
     * range.from();   //   10
     * range.to();     //   99
     * range.length(); //   90
     * @endcode
     *
     */
    qint64 length() const;

    /**
     * @brief Returns dataSize of range.
     *
     * If dataSize is not set, returns -1.
     */
    qint64 dataSize() const;

    /**
     * @brief Checks if range is valid
     *
     * Range is considered invalid if it is out of bounds, that is when this
     * inequality is false - (from <= to < dataSize).
     * When QHttpRange(const QString&) fails to parse range string, resulting
     * range is also considered invalid.
     *
     * Example:
     * @code
     * QHttpRange range(1, 0, -1);
     * range.isValid(); // false
     *
     * range = QHttpRange(512, 1024);
     * range.isValid(); // true
     *
     * range = QHttpRange("-");
     * range.isValid(); // false
     *
     * range = QHttpRange("abccbf");
     * range.isValid(); // false
     *
     * range = QHttpRange(0, 512, 128);
     * range.isValid(); // false
     *
     * range = QHttpRange(128, 64, 512);
     * range.isValid(); // false
     * @endcode
     */
    bool isValid() const;

    /**
     * @brief Returns representation suitable for Content-Range header.
     *
     * Example:
     * @code
     * QHttpRange range(0, 100, 1000);
     * range.contentRange(); // "0-100/1000"
     *
     * // When resource size is unknown
     * range = QHttpRange(512, 1024);
     * range.contentRange(); // "512-1024/*"
     *
     * // if range request was bad, return resource size
     * range = QHttpRange(1, 0, 1200);
     * range.contentRange(); // "*\/1200"
     * @endcode
     */
    QString contentRange() const;

private:

    QHttpRangePrivate *const d;
};

#endif // QHTTPENGINE_QHTTPRANGE_H
