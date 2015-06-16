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

#ifndef QHTTPENGINE_QHTTPPARSER_H
#define QHTTPENGINE_QHTTPPARSER_H

#include <QList>

#include "../core/qhttpheader.h"
#include "config.h"

/**
 * @brief Utility methods for parsing HTTP requests and responses
 */
class QHTTPENGINE_EXPORT QHttpParser
{
public:

    /**
     * @brief Split a QByteArray by the provided delimiter
     *
     * If the delimiter is not present in the QByteArray, a list will be
     * returned containing the original QByteArray as its only element. The
     * delimiter must not be empty.
     *
     * If a value is provided for maxSplit, the list will contain no more than
     * maxSplit + 1 items.
     */
    static QList<QByteArray> split(const QByteArray &data, const QByteArray &delim, int maxSplit = 0);

    /**
     * @brief Parse an HTTP request into its components
     *
     * This method will parse the headers from an HTTP request (everything up
     * to the "\r\n\r\n" terminator) and store the values in the specified
     * references.
     */
    static bool parseRequest(const QByteArray &data, QByteArray &method, QByteArray &path, QList<QHttpHeader> &headers);
};

#endif // QHTTPENGINE_QHTTPPARSER_H
