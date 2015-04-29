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

#include "qhttpresponse.h"
#include "qhttpresponse_p.h"

QHttpResponsePrivate::QHttpResponsePrivate(QHttpResponse *response, QIODevice *baseDevice)
    : QObject(response),
      q(response),
      device(baseDevice),
      headersWritten(false)
{
    connect(device, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));
}

void QHttpResponsePrivate::onBytesWritten(qint64 bytes)
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

QHttpResponse::QHttpResponse(QIODevice *device, QObject *parent)
    : QIODevice(parent),
      d(new QHttpResponsePrivate(this, device))
{
    setOpenMode(QIODevice::WriteOnly);
}

bool QHttpResponse::isSequential() const
{
    return true;
}

void QHttpResponse::writeHeaders()
{
    // Use a QByteArray for building the header so that we
    // can later determine exactly how many bytes were written
    QByteArray header;

    // Append the status line
    header.append("HTTP/1.0 ");
    header.append(d->statusCode);
    header.append("\r\n");

    // Append each of the headers followed by a CRLF
    for(QMap<QByteArray, QByteArray>::const_iterator i = d->headers.constBegin();
            i != d->headers.constEnd(); ++i) {
        header.append(i.key());
        header.append(": ");
        header.append(i.value());
        header.append("\r\n");
    }

    // Append an extra CRLF
    header.append("\r\n");

    // Write the header
    d->device->write(header);

    d->headerLength = header.length();
    d->headersWritten = true;
}

void QHttpResponse::setStatusCode(const QByteArray &statusCode)
{
    d->statusCode = statusCode;
}

void QHttpResponse::setHeader(const QByteArray &name, const QByteArray &value)
{
    d->headers.insert(name, value);
}

qint64 QHttpResponse::readData(char *, qint64)
{
    // The device cannot be read from
    return -1;
}

qint64 QHttpResponse::writeData(const char *data, qint64 len)
{
    // If the response headers have not yet been written, they
    // must immediately be written before the data can be
    if(!d->headersWritten) {
        writeHeaders();
    }

    return d->device->write(data, len);
}
