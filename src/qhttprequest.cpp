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

#include "qhttprequest.h"
#include "qhttprequest_p.h"

QHttpRequestPrivate::QHttpRequestPrivate(QHttpRequest *request, QIODevice *baseDevice)
    : QObject(request),
      q(request),
      device(baseDevice),
      error(QHttpRequest::NoError),
      headersParsed(false)
{
    connect(device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // Attempt to read data from the device the next time the event loop is entered
    QTimer::singleShot(0, this, SLOT(onReadyRead()));
}

void QHttpRequestPrivate::onReadyRead()
{
    // Add the new data to the internal buffer
    buffer.append(device->readAll());

    // If the request headers have not yet been parsed, check for two CRLFs
    if(!headersParsed) {

        int index = buffer.indexOf("\r\n\r\n");
        if(index != -1) {

            // Parse the headers and remove them from the data
            parseHeaders(buffer.left(index));
            buffer.remove(0, index + 4);

            // Check to see if an error occurred during parsing
            if(error != QHttpRequest::NoError) {

                // Set a descriptive error message
                switch(error) {
                case QHttpRequest::MalformedRequestLine:
                    q->setErrorString(tr("Malformed request line"));
                    break;
                case QHttpRequest::MalformedRequestHeader:
                    q->setErrorString(tr("Malformed request header"));
                    break;
                case QHttpRequest::InvalidHttpVersion:
                    q->setErrorString(tr("Invalid HTTP version"));
                    break;
                default:
                    break;
                }

                Q_EMIT q->errorChanged(error);

            } else {

                // Indicate that the request headers were parsed
                Q_EMIT q->headersParsedChanged(headersParsed = true);
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

QList<QByteArray> QHttpRequestPrivate::split(const QByteArray &data, const QByteArray &delim)
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

void QHttpRequestPrivate::parseHeaders(const QByteArray &data)
{
    // Split the header into individual lines
    QList<QByteArray> lines = split(data, "\r\n");

    // Parse the status line
    parseStatusLine(lines.takeFirst());

    // Parse each of the remaining lines (the headers)
    foreach(const QByteArray &line, lines) {
        parseHeader(line);
    }
}

void QHttpRequestPrivate::parseStatusLine(const QByteArray &line)
{
    // The request line consists of three parts separated by space
    QList<QByteArray> parts = line.split(' ');

    // If 3 parts are not supplied, then stop parsing the line
    // to avoid invalid array indices later on
    if(parts.count() != 3) {
        error = QHttpRequest::MalformedRequestLine;
        return;
    }

    // Only HTTP versions 1.0 and 1.1 are currently supported
    if(parts[2] != "HTTP/1.0" && parts[2] != "HTTP/1.1") {
        error = QHttpRequest::InvalidHttpVersion;
    }

    method = parts[0];
    path = parts[1];
}

void QHttpRequestPrivate::parseHeader(const QByteArray &line)
{
    // Each header consists of the key, ":", and the value
    int index = line.indexOf(":");

    // If the colon was not found, then stop parsing the header
    if(index == -1) {
        error = QHttpRequest::MalformedRequestHeader;
        return;
    }

    // Trim each part and add it to the map
    headers.insert(
        line.left(index).trimmed(),
        line.mid(index + 1).trimmed()
    );
}

QHttpRequest::QHttpRequest(QIODevice *device, QObject *parent)
    : QIODevice(parent),
      d(new QHttpRequestPrivate(this, device))
{
    setOpenMode(QIODevice::ReadOnly);
}

qint64 QHttpRequest::bytesAvailable() const
{
    return d->buffer.size() + QIODevice::bytesAvailable();
}

bool QHttpRequest::isSequential() const
{
    return true;
}

QHttpRequest::Error QHttpRequest::error() const
{
    return d->error;
}

QByteArray QHttpRequest::method() const
{
    return d->method;
}

QByteArray QHttpRequest::path() const
{
    return d->path;
}

bool QHttpRequest::headersParsed() const
{
    return d->headersParsed;
}

QList<QByteArray> QHttpRequest::headers() const
{
    return d->headers.keys();
}

QByteArray QHttpRequest::header(const QByteArray &name) const
{
    for(QMap<QByteArray, QByteArray>::const_iterator i = d->headers.constBegin();
            i != d->headers.constEnd(); ++i) {
        if(i.key().toLower() == name.toLower()) {
            return i.value();
        }
    }

    return QByteArray();
}

qint64 QHttpRequest::readData(char *data, qint64 maxlen)
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

qint64 QHttpRequest::writeData(const char *, qint64)
{
    // The device cannot be written to
    return -1;
}
