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

#include <cstring>

#include <QTimer>

#include "../util/qhttpparser.h"
#include "qhttpsocket.h"
#include "qhttpsocket_p.h"

QHttpSocketPrivate::QHttpSocketPrivate(QHttpSocket *socket, QTcpSocket *baseSocket)
    : QObject(socket),
      q(socket),
      socket(baseSocket),
      statusCode("200 OK"),
      headersParsed(false),
      headersWritten(false)
{
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));

    // Attempt to read data from the device the next time the event loop is entered
    QTimer::singleShot(0, this, SLOT(onReadyRead()));
}

void QHttpSocketPrivate::onReadyRead()
{
    // Add the new data to the internal buffer
    buffer.append(socket->readAll());

    // If the request headers have not yet been parsed, check for two CRLFs
    if(!headersParsed) {

        int index = buffer.indexOf("\r\n\r\n");
        if(index != -1) {

            // Parse the headers and remove them from the data
            QList<QByteArray> parts;
            if(QHttpParser::parseHeaders(buffer.left(index), parts, requestHeaders)) {

                // Remove the headers from the buffer
                buffer.remove(0, index + 4);

                // Store the method and path
                method = parts[0];
                path = parts[1];

                // Indicate that the request headers were parsed
                headersParsed = true;
                Q_EMIT q->headersParsedChanged();

            } else {
                Q_EMIT q->error();
            }

        } else {
            return;
        }
    }

    // If there is data remaining in the buffer, emit the readyRead signal
    if(buffer.length()) {
        Q_EMIT q->readyRead();
    }
}

void QHttpSocketPrivate::onBytesWritten(qint64 bytes)
{
    // Since this signal may be emitted even if no actual data was written,
    // the number of bytes needs to be subtracted from the header length
    if(headerLength - bytes >= 0) {
        headerLength -= bytes;
    } else {
        Q_EMIT q->bytesWritten(bytes - headerLength);
        headerLength = 0;
    }
}

QHttpSocket::QHttpSocket(QTcpSocket *socket, QObject *parent)
    : QIODevice(parent),
      d(new QHttpSocketPrivate(this, socket))
{
    setOpenMode(QIODevice::ReadWrite);
}

qint64 QHttpSocket::bytesAvailable() const
{
    return d->buffer.size() + QIODevice::bytesAvailable();
}

bool QHttpSocket::isSequential() const
{
    return true;
}

QByteArray QHttpSocket::method() const
{
    return d->method;
}

QByteArray QHttpSocket::path() const
{
    return d->path;
}

bool QHttpSocket::headersParsed() const
{
    return d->headersParsed;
}

QList<QIByteArray> QHttpSocket::headers() const
{
    return d->requestHeaders.keys();
}

QByteArray QHttpSocket::header(const QByteArray &name) const
{
    return d->requestHeaders.value(name);
}

void QHttpSocket::setStatusCode(const QByteArray &statusCode)
{
    d->statusCode = statusCode;
}

void QHttpSocket::setHeader(const QByteArray &name, const QByteArray &value)
{
    d->responseHeaders.insert(name, value);
}

void QHttpSocket::writeHeaders()
{
    // Use a QByteArray for building the header so that we
    // can later determine exactly how many bytes were written
    QByteArray header;

    // Append the status line
    header.append("HTTP/1.0 ");
    header.append(d->statusCode);
    header.append("\r\n");

    // Append each of the headers followed by a CRLF
    for(QHttpHeaderMap::const_iterator i = d->responseHeaders.constBegin(); i != d->responseHeaders.constEnd(); ++i) {
        header.append(i.key());
        header.append(": ");
        header.append(i.value());
        header.append("\r\n");
    }

    // Append an extra CRLF
    header.append("\r\n");

    d->headerLength = header.length();
    d->headersWritten = true;

    // Write the header
    d->socket->write(header);
}

qint64 QHttpSocket::readData(char *data, qint64 maxlen)
{
    // Data can only be read from the device once the request headers are parsed
    if(!headersParsed()) {
        return 0;
    }

    // Ensure that no more than the requested amount or the size of the buffer is read
    qint64 size = qMin(static_cast<qint64>(d->buffer.size()), maxlen);
    memcpy(data, d->buffer.constData(), size);

    // Remove the amount that was read from the buffer
    d->buffer.remove(0, size);

    return size;
}

qint64 QHttpSocket::writeData(const char *data, qint64 len)
{
    // If the response headers have not yet been written, they must
    // immediately be written before the data can be
    if(!d->headersWritten) {
        writeHeaders();
    }

    return d->socket->write(data, len);
}
