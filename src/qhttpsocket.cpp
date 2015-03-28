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

QHttpSocketPrivate::QHttpSocketPrivate(QHttpSocket *httpSocket)
    : q(httpSocket),
      readingHeader(true)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void QHttpSocketPrivate::onReadyRead()
{
    buffer.append(socket.readAll());

    if(readingHeader) {

        // Check for two successive CRLF sequences in the input
        int index = buffer.indexOf("\r\n\r\n");
        if(index != -1) {

            parseHeaders(buffer.left(index));

            buffer.remove(0, index + 4);
            readingHeader = false;

            Q_EMIT q->requestHeadersParsed();
        }

    } else {
        Q_EMIT q->readyRead();
    }
}

void QHttpSocketPrivate::parseHeaders(const QByteArray &headers)
{
    // Read and parse the status line
    int index = headers.indexOf("\r\n");
    if(index == -1) {
        error = QHttpSocket::MalformedStatusLine;
        q->setErrorString(tr("Malformed status line"));
        Q_EMIT q->error();
    }
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

QHttpSocket::Error QHttpSocket::error() const
{
    return d->error;
}

QString QHttpSocket::requestMethod() const
{
    return d->requestMethod;
}

QString QHttpSocket::requestPath() const
{
    return d->requestPath;
}

QString QHttpSocket::requestHeader(const QString &header) const
{
    return d->requestHeaders.value(header);
}

QStringList QHttpSocket::requestHeaders() const
{
    return d->requestHeaders.keys();
}

void QHttpSocket::setResponseStatusCode(const QString &statusCode)
{
    d->responseStatusCode = statusCode;
}

void QHttpSocket::setResponseHeader(const QString &header, const QString &value)
{
    d->responseHeaders.insert(header, value);
}

bool QHttpSocket::isSequential() const
{
    return true;
}

qint64 QHttpSocket::readData(char *data, qint64 maxlen)
{
    //...

    return 0;
}

qint64 QHttpSocket::writeData(const char *data, qint64 len)
{
    //...

    return 0;
}
