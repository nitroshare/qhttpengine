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

#include <QByteArray>

#include "QHttpEngine/qhttpparser.h"

void QHttpParser::split(const QByteArray &data, const QByteArray &delim, int maxSplit, QList<QByteArray> &parts)
{
    int index = 0;

    for(int i = 0; !maxSplit || i < maxSplit; ++i) {
        int nextIndex = data.indexOf(delim, index);

        if(nextIndex == -1) {
            break;
        }

        parts.append(data.mid(index, nextIndex - index));
        index = nextIndex + delim.length();
    }

    // Append whatever remains to the list
    parts.append(data.mid(index));
}

bool QHttpParser::parseHeaderList(const QList<QByteArray> &lines, QHttpHeaderMap &headers)
{
    foreach(const QByteArray &line, lines) {

        QList<QByteArray> parts;
        split(line, ":", 1, parts);

        // Ensure that the delimiter (":") was encountered at least once
        if(parts.count() != 2) {
            return false;
        }

        // Trim excess whitespace and add the header to the list
        headers.insert(parts[0].trimmed(), parts[1].trimmed());
    }

    return true;
}

bool QHttpParser::parseHeaders(const QByteArray &data, QList<QByteArray> &parts, QHttpHeaderMap &headers)
{
    // Split the data into individual lines
    QList<QByteArray> lines;
    split(data, "\r\n", 0, lines);

    // Split the first line into a maximum of three parts
    split(lines.takeFirst(), " ", 2, parts);
    if(parts.count() != 3) {
        return false;
    }

    return parseHeaderList(lines, headers);
}

bool QHttpParser::parseRequestHeaders(const QByteArray &data, QByteArray &method, QByteArray &path, QHttpHeaderMap &headers)
{
    QList<QByteArray> parts;
    if(!parseHeaders(data, parts, headers)) {
        return false;
    }

    // Only HTTP/1.x versions are supported for now
    if(parts[2] != "HTTP/1.0" && parts[2] != "HTTP/1.1") {
        return false;
    }

    method = parts[0];
    path = parts[1];

    return true;
}

bool QHttpParser::parseResponseHeaders(const QByteArray &data, int &statusCode, QByteArray &statusReason, QHttpHeaderMap &headers)
{
    QList<QByteArray> parts;
    if(!parseHeaders(data, parts, headers)) {
        return false;
    }

    statusCode = parts[1].toInt();
    statusReason = parts[2];

    // Ensure a valid status code
    return statusCode >= 100 && statusCode <= 599;
}
