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

#include <cstring>

#include <QTimer>

#include "qhttpsocket.h"
#include "qhttpsocket_p.h"

QHttpSocketPrivate::QHttpSocketPrivate(QHttpSocket *httpSocket, QTcpSocket *baseSocket)
    : q(httpSocket),
      socket(baseSocket),
      httpError(QHttpSocket::NoError),
      requestHeadersRead(false),
      responseStatusCode("200 OK"),
      responseHeaderLength(0),
      responseHeadersWritten(false)
{
    // Re-parent the socket to this class
    socket->setParent(this);

    // Both of these signals must be handled directly since the
    // socket acts as a transport both for headers and for data
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));

    // Next time the event loop is entered, attempt to read data from the socket
    QTimer::singleShot(0, this, SLOT(onReadyRead()));
}

void QHttpSocketPrivate::writeResponseHeaders()
{
    // Use a QByteArray for building the header so that we
    // can later determine exactly how many bytes were written
    QByteArray header;

    // Append the status line
    header.append("HTTP/1.0 ");
    header.append(responseStatusCode.toUtf8());
    header.append("\r\n");

    // Append each of the headers followed by a CRLF
    QMap<QString, QString>::const_iterator i = responseHeaders.constBegin();
    for(; i != responseHeaders.constEnd(); ++i) {
        header.append(i.key().toUtf8());
        header.append(": ");
        header.append(i.value().toUtf8());
        header.append("\r\n");
    }

    // Append an extra CRLF
    header.append("\r\n");

    // Write the header
    socket->write(header);

    responseHeaderLength = header.length();
    responseHeadersWritten = true;
}

void QHttpSocketPrivate::onReadyRead()
{
    // Add the new data to the internal buffer
    buffer.append(socket->readAll());

    // If the request headers have not yet been parsed, check for two CRLFs
    if(!requestHeadersRead) {

        int index = buffer.indexOf("\r\n\r\n");
        if(index != -1) {

            // Parse the headers and remove them from the data
            parseRequestHeaders(buffer.left(index));
            buffer.remove(0, index + 4);

            // Check to see if an error occurred during parsing
            if(httpError != QHttpSocket::NoError) {

                // Set a descriptive error message
                switch(httpError) {
                case QHttpSocket::MalformedRequestLine:
                    q->setErrorString(tr("Malformed request line"));
                    break;
                case QHttpSocket::MalformedRequestHeader:
                    q->setErrorString(tr("Malformed request header"));
                    break;
                case QHttpSocket::InvalidHttpVersion:
                    q->setErrorString(tr("Invalid HTTP version"));
                    break;
                }

                Q_EMIT q->httpErrorChanged(httpError);

            } else {

                // Indicate that the request headers were parsed
                Q_EMIT q->requestHeadersReadChanged(requestHeadersRead = true);
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
    if(responseHeaderLength - bytes >= 0) {
        responseHeaderLength -= bytes;
    } else {
        Q_EMIT q->bytesWritten(bytes - responseHeaderLength);
        responseHeaderLength = 0;
    }
}

void QHttpSocketPrivate::parseRequestHeaders(const QString &headers)
{
    // Split the header into individual lines
    QStringList parts = headers.split("\r\n");

    // Parse the first line (the request line)
    parseRequestLine(parts.takeFirst());

    // Parse each of the remaining lines (the headers)
    foreach(const QString &header, parts) {
        parseRequestHeader(header);
    }
}

void QHttpSocketPrivate::parseRequestLine(const QString &line)
{
    // The request line consists of three parts separated by space
    QStringList parts = line.split(" ");

    // If 3 parts are not supplied, then stop parsing the line
    // to avoid invalid array indices later on
    if(parts.count() != 3) {
        httpError = QHttpSocket::MalformedRequestLine;
        return;
    }

    // Only HTTP versions 1.0 and 1.1 are currently supported
    if(parts[2] != "HTTP/1.0" && parts[2] != "HTTP/1.1") {
        httpError = QHttpSocket::InvalidHttpVersion;
    }

    requestMethod = parts[0];
    requestPath = parts[1];
}

void QHttpSocketPrivate::parseRequestHeader(const QString &header)
{
    // Each header consists of the key, ":", and the value
    int index = header.indexOf(":");

    // If the colon was not found, then stop parsing the header
    if(index == -1) {
        httpError = QHttpSocket::MalformedRequestHeader;
        return;
    }

    // Trim each part and add it to the map
    requestHeaders.insert(
        header.left(index).trimmed().toLower(),
        header.mid(index + 1).trimmed()
    );
}

QHttpSocket::QHttpSocket(QTcpSocket *socket, QObject *parent)
    : QIODevice(parent),
      d(new QHttpSocketPrivate(this, socket))
{
    // The device is immediately open for reading
    setOpenMode(QIODevice::ReadWrite);
}

QHttpSocket::~QHttpSocket()
{
    delete d;
}

qint64 QHttpSocket::bytesAvailable() const
{
    return d->buffer.size() + QIODevice::bytesAvailable();
}

void QHttpSocket::close()
{
    // Don't do anything if the device was already closed
    if(!isOpen()) {
        return;
    }

    // If the response headers have not yet been written, then do so before closing
    if(!d->responseHeadersWritten) {
        d->writeResponseHeaders();
    }

    d->socket->close();
    QIODevice::close();
}

bool QHttpSocket::isSequential() const
{
    return true;
}

QHttpSocket::HttpError QHttpSocket::httpError() const
{
    return d->httpError;
}

QString QHttpSocket::requestMethod() const
{
    return d->requestMethod;
}

QString QHttpSocket::requestPath() const
{
    return d->requestPath;
}

QStringList QHttpSocket::requestHeaders() const
{
    return d->requestHeaders.keys();
}

bool QHttpSocket::requestHeadersRead() const
{
    return d->requestHeadersRead;
}

QString QHttpSocket::requestHeader(const QString &header) const
{
    return d->requestHeaders.value(header.toLower());
}

void QHttpSocket::setResponseStatusCode(const QString &statusCode)
{
    d->responseStatusCode = statusCode;
}

void QHttpSocket::setResponseHeader(const QString &header, const QString &value)
{
    d->responseHeaders.insert(header, value);
}

qint64 QHttpSocket::readData(char *data, qint64 maxlen)
{
    // Data can only be read from the socket once the request headers are read
    if(!requestHeadersRead()) {
        return -1;
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
    // If the response headers have not yet been written, they
    // must immediately be written before the data can be
    if(!d->responseHeadersWritten) {
        d->writeResponseHeaders();
    }

    return d->socket->write(data, len);
}
