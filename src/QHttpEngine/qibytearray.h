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

#ifndef QHTTPENGINE_QIBYTEARRAY_H
#define QHTTPENGINE_QIBYTEARRAY_H

#include <QByteArray>

#include "qhttpengine.h"

/**
 * @brief Case-insensitive subclass of QByteArray
 * @headerfile qibytearray.h QHttpEngine/QIByteArray
 *
 * The QIByteArray is identical to the QByteArray class in all aspects except
 * that it performs comparisons in a case-insensitive manner.
 */
class QHTTPENGINE_EXPORT QIByteArray : public QByteArray
{
public:

    /**
     * @brief Create an empty QIByteArray
     */
    QIByteArray();

    /**
     * @brief QIByteArray copy constructor
     */
    QIByteArray(const QByteArray &other);

    /**
     * @brief Create a QIByteArray from a const char *
     */
    QIByteArray(const char *data, int size = -1);
};

QHTTPENGINE_EXPORT bool operator==(const QIByteArray &a1, const QIByteArray &a2);
QHTTPENGINE_EXPORT bool operator==(const QIByteArray &a1, const QString &a2);
QHTTPENGINE_EXPORT bool operator==(const QString &a1, const QIByteArray &a2);
QHTTPENGINE_EXPORT bool operator==(const QIByteArray &a1, const QByteArray &a2);
QHTTPENGINE_EXPORT bool operator==(const QByteArray &a1, const QIByteArray &a2);
QHTTPENGINE_EXPORT bool operator==(const QIByteArray &a1, const char *a2);
QHTTPENGINE_EXPORT bool operator==(const char *a1, const QIByteArray &a2);

#endif // QHTTPENGINE_QIBYTEARRAY_H
