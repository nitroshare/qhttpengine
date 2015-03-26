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
 **/

#include "qhttpsocket.h"
#include "qhttpsocket_p.h"

QHttpSocketPrivate::QHttpSocketPrivate(QHttpSocket *socket)
    : q(socket)
{
}

QHttpSocket::QHttpSocket(qintptr socketDescriptor, QObject *parent)
    : QIODevice(parent),
      d(new QHttpSocketPrivate(this))
{
    d->socket.setSocketDescriptor(socketDescriptor);
}

QHttpSocket::~QHttpSocket()
{
    delete d;
}

QByteArray QHttpSocket::method() const
{
    return d->method;
}

QByteArray QHttpSocket::path() const
{
    return d->path;
}

QByteArray QHttpSocket::statusCode() const
{
    return d->statusCode;
}

void QHttpSocket::setStatusCode(const QByteArray &code)
{
    d->statusCode = code;
}

qint64 QHttpSocket::readData(char *data, qint64 maxlen)
{
    //...
}

qint64 QHttpSocket::writeData(const char *data, qint64 len)
{
    //...
}
