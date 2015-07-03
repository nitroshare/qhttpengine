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

#include "qsubhandler.h"
#include "qsubhandler_p.h"

QSubHandlerPrivate::QSubHandlerPrivate(QSubHandler *handler)
    : QObject(handler),
      q(handler)
{
}

QSubHandler::QSubHandler(QObject *parent)
    : QHttpHandler(parent),
      d(new QSubHandlerPrivate(this))
{
}

void QSubHandler::addHandler(const QRegExp &pattern, QHttpHandler *handler)
{
    d->patterns.append(URL(pattern, handler));
}

void QSubHandler::process(QHttpSocket *socket, const QString &path)
{
    // Check each of the patterns for a match
    foreach(URL url, d->patterns) {
        if(url.first.indexIn(path) != -1) {
            url.second->process(socket, path.mid(url.first.matchedLength()));
            return;
        }
    }

    // If no match was found, indicate an error
    socket->writeError(QHttpSocket::NotFound);
}
