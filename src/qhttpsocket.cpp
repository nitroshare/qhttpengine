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

QHttpSocketPrivate::QHttpSocketPrivate(QHttpSocket *httpSocket, QAbstractSocket *baseSocket)
    : q(httpSocket),
      socket(baseSocket),
      error(QHttpSocket::None),
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

    // Also, the error signal needs to be handled
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

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

        } else {
            return;
        }
    }

    // If there is data in the buffer, emit the readyRead signal
    if(buffer.length()) {
        Q_EMIT q->readyRead();
    }
}

void QHttpSocketPrivate::onBytesWritten(qint64 bytes)
{
    // Since this signal may be emitted even if no data was actually written,
    // the number of bytes needs to be subtracted from the header length
    if(responseHeaderLength - bytes >= 0) {
        responseHeaderLength -= bytes;
    } else {
        Q_EMIT q->bytesWritten(bytes - responseHeaderLength);
        responseHeaderLength = 0;
    }
}

void QHttpSocketPrivate::onError(QAbstractSocket::SocketError socketError)
{
    // If the "error" was simply the client disconnecting, then check
    // to see if the headers were received - otherwise it's a protocol error
    if(socketError == QAbstractSocket::RemoteHostClosedError) {

        if(!requestHeadersRead) {
            abortWithError(QHttpSocket::IncompleteHeader);
        }

    } else {
        abortWithError(QHttpSocket::SocketError);
    }

    // Indicate that the device is no longer open
    q->setOpenMode(QIODevice::NotOpen);
}

void QHttpSocketPrivate::abortWithError(QHttpSocket::Error socketError)
{
    switch(socketError) {
    case QHttpSocket::MalformedRequestLine:
        q->setErrorString(tr("Malformed request line"));
        break;
    case QHttpSocket::MalformedRequestHeader:
        q->setErrorString(tr("Malformed request header"));
        break;
    case QHttpSocket::InvalidHttpVersion:
        q->setErrorString(tr("Invalid HTTP version"));
        break;
    case QHttpSocket::IncompleteHeader:
        q->setErrorString(tr("Incomplete header received"));
        break;
    case QHttpSocket::SocketError:
        q->setErrorString(socket->errorString());
        break;
    }

    Q_EMIT q->errorChanged(error = socketError);
}

void QHttpSocketPrivate::parseRequestHeaders(const QString &headers)
{
    // Split the header into individual lines
    QStringList parts = headers.split("\r\n");

    // Parse the first line (the request line)
    parseRequestLine(parts.takeFirst());

    // Parse each of the remaining lines (the headers)
    foreach(QString header, parts) {
        parseRequestHeader(header);
    }

    requestHeadersRead = true;
    Q_EMIT q->requestHeadersParsed();
}

void QHttpSocketPrivate::parseRequestLine(const QString &line)
{
    // The request line consists of three parts separated by space
    QStringList parts = line.split(" ");
    if(parts.count() != 3) {
        abortWithError(QHttpSocket::MalformedRequestLine);
        return;
    }

    // Only HTTP versions 1.0 and 1.1 are currently supported
    if(parts[2] != "HTTP/1.0" && parts[2] != "HTTP/1.1") {
        abortWithError(QHttpSocket::InvalidHttpVersion);
        return;
    }

    requestMethod = parts[0];
    requestUri = parts[1];
}

void QHttpSocketPrivate::parseRequestHeader(const QString &header)
{
    // Each header consists of the key, ":", and the value
    int index = header.indexOf(":");
    if(index == -1) {
        abortWithError(QHttpSocket::MalformedRequestHeader);
        return;
    }

    // Trim each part and add it to the map
    requestHeaders.insert(
        header.left(index).trimmed().toLower(),
        header.mid(index + 1).trimmed()
    );
}

QHttpSocket::QHttpSocket(QAbstractSocket *socket, QObject *parent)
    : QIODevice(parent),
      d(new QHttpSocketPrivate(this, socket))
{
    setOpenMode(QIODevice::ReadWrite);
}

QHttpSocket::~QHttpSocket()
{
    delete d;
}

void QHttpSocket::close()
{
    // If the response headers have not yet been written, then do so before closing
    if(!d->responseHeadersWritten) {
        d->writeResponseHeaders();
    }

    d->socket->close();
    setOpenMode(QIODevice::NotOpen);
}

QHttpSocket::Error QHttpSocket::error() const
{
    return d->error;
}

QString QHttpSocket::requestMethod() const
{
    if(!d->requestHeadersRead) {
        qWarning("Request headers have not yet been read");
    }

    return d->requestMethod;
}

QString QHttpSocket::requestUri() const
{
    if(!d->requestHeadersRead) {
        qWarning("Request headers have not yet been read");
    }

    return d->requestUri;
}

QStringList QHttpSocket::requestHeaders() const
{
    if(!d->requestHeadersRead) {
        qWarning("Request headers have not yet been read");
    }

    return d->requestHeaders.keys();
}

QString QHttpSocket::requestHeader(const QString &header) const
{
    if(!d->requestHeadersRead) {
        qWarning("Request headers have not yet been read");
    }

    return d->requestHeaders.value(header.toLower());
}

void QHttpSocket::setResponseStatusCode(const QString &statusCode)
{
    if(d->responseHeadersWritten) {
        qWarning("Response headers have already been written");
    }

    d->responseStatusCode = statusCode;
}

void QHttpSocket::setResponseHeader(const QString &header, const QString &value)
{
    if(d->responseHeadersWritten) {
        qWarning("Response headers have already been written");
    }

    d->responseHeaders.insert(header, value);
}

bool QHttpSocket::isSequential() const
{
    return true;
}

qint64 QHttpSocket::readData(char *data, qint64 maxlen)
{
    // Data can only be read from the socket once the request headers are read
    if(!d->requestHeadersRead) {
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
