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

#include <cstring>

#include "QHttpEngine/qhttpsocket.h"
#include "qhttpsocket_p.h"

// Predefined error response requires a simple HTML template to be returned to
// the client describing the error condition
const QString ErrorTemplate =
        "<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<title>%1 %2</title></head><body><h1>%1 %2</h1><p>"
        "An error has occurred while trying to display the requested resource. "
        "Please contact the website owner if this error persists."
        "</p><hr><p><em>QHttpEngine %3</em></p></body></html>";

QHttpSocketPrivate::QHttpSocketPrivate(QHttpSocket *httpSocket, QTcpSocket *tcpSocket)
    : QObject(httpSocket),
      q(httpSocket),
      socket(tcpSocket),
      readState(ReadHeaders),
      requestDataRead(0),
      requestDataTotal(-1),
      writeState(WriteNone),
      responseStatusCode(200),
      responseStatusReason(statusReason(200))
{
    socket->setParent(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));

    // Process anything already received by the socket
    onReadyRead();
}

QByteArray QHttpSocketPrivate::statusReason(int statusCode) const
{
    switch(statusCode) {
    case QHttpSocket::OK: return "OK";
    case QHttpSocket::MovedPermanently: return "MOVED PERMANENTLY";
    case QHttpSocket::Found: return "FOUND";
    case QHttpSocket::BadRequest: return "BAD REQUEST";
    case QHttpSocket::Forbidden: return "FORBIDDEN";
    case QHttpSocket::NotFound: return "NOT FOUND";
    case QHttpSocket::MethodNotAllowed: return "METHOD NOT ALLOWED";
    case QHttpSocket::InternalServerError: return "INTERNAL SERVER ERROR";
    default: return "UNKNOWN";
    }
}

void QHttpSocketPrivate::onReadyRead()
{
    // Append all of the new data to the read buffer
    readBuffer.append(socket->readAll());

    // If reading headers, return if they could not be read (yet)
    if(readState == ReadHeaders) {
        if(!readHeaders()) {
            return;
        }
    }

    if(readState == ReadData) {
        readData();
    } else if(readState == ReadFinished) {

        // Any data received here is unexpected and should be ignored
        readBuffer.clear();
    }
}

void QHttpSocketPrivate::onBytesWritten(qint64 bytes)
{
    // Check to see if all of the response header was written
    if(writeState == WriteHeaders) {
        if(responseHeaderRemaining - bytes > 0) {
            responseHeaderRemaining -= bytes;
        } else {
            writeState = WriteData;
            bytes -= responseHeaderRemaining;
        }
    }

    if(writeState == WriteData) {
        Q_EMIT q->bytesWritten(bytes);
    }
}

bool QHttpSocketPrivate::readHeaders()
{
    // Check for the double CRLF that signals the end of the headers and
    // if it is not found, wait until the next time readyRead is emitted
    int index = readBuffer.indexOf("\r\n\r\n");
    if(index == -1) {
        return false;
    }

    // Attempt to parse the headers and if a problem is encountered, abort
    // the connection (so that no more data is read or written) and return
    if(!QHttpParser::parseRequestHeaders(readBuffer.left(index), requestMethod, requestPath, requestHeaders)) {
        q->writeError(QHttpSocket::BadRequest);
        return false;
    }

    // Remove the headers from the buffer
    readBuffer.remove(0, index + 4);

    // Check for the content-length header - if it is present, then
    // prepare to read the specified amount of data, otherwise, no data
    // should be read from the socket and the read channel is finished
    if(requestHeaders.contains("Content-Length")) {
        readState = ReadData;
        requestDataTotal = requestHeaders.value("Content-Length").toLongLong();
    } else {
        readState = ReadFinished;
    }

    // Indicate that the headers have been parsed
    Q_EMIT q->headersParsed();

    // If the new readState is ReadFinished, then indicate so
    if(readState == ReadFinished) {
        Q_EMIT q->readChannelFinished();
    }

    return true;
}

void QHttpSocketPrivate::readData()
{
    // Emit the readyRead() signal if any data is available in the buffer
    if(readBuffer.size()) {
        Q_EMIT q->readyRead();
    }

    // Check to see if the specified amount of data has been read from the
    // socket, if so, emit the readChannelFinished() signal
    if(requestDataRead + readBuffer.size() >= requestDataTotal) {
        readState = ReadFinished;
        Q_EMIT q->readChannelFinished();
    }
}

QHttpSocket::QHttpSocket(QTcpSocket *socket, QObject *parent)
    : QIODevice(parent),
      d(new QHttpSocketPrivate(this, socket))
{
    // The device is initially open for both reading and writing
    setOpenMode(QIODevice::ReadWrite);
}

qint64 QHttpSocket::bytesAvailable() const
{
    if(d->readState > QHttpSocketPrivate::ReadHeaders) {
        return d->readBuffer.size() + QIODevice::bytesAvailable();
    } else {
        return 0;
    }
}

bool QHttpSocket::isSequential() const
{
    return true;
}

void QHttpSocket::close()
{
    // Invoke the parent method
    QIODevice::close();

    d->readState = QHttpSocketPrivate::ReadFinished;
    d->writeState = QHttpSocketPrivate::WriteFinished;

    d->socket->close();
}

QByteArray QHttpSocket::method() const
{
    return d->requestMethod;
}

QByteArray QHttpSocket::path() const
{
    return d->requestPath;
}

bool QHttpSocket::isHeadersParsed() const
{
    return d->readState > QHttpSocketPrivate::ReadHeaders;
}

QHttpHeaderMap QHttpSocket::headers() const
{
    return d->requestHeaders;
}

qint64 QHttpSocket::contentLength() const
{
    return d->requestDataTotal;
}

void QHttpSocket::setStatusCode(int statusCode, const QByteArray &statusReason)
{
    d->responseStatusCode = statusCode;
    d->responseStatusReason = statusReason.isNull() ? d->statusReason(statusCode) : statusReason;
}

void QHttpSocket::setHeader(const QByteArray &name, const QByteArray &value)
{
    d->responseHeaders.insert(name, value);
}

void QHttpSocket::setHeaders(const QHttpHeaderMap &headers)
{
    d->responseHeaders = headers;
}

void QHttpSocket::writeHeaders()
{
    // Use a QByteArray for building the header so that we can later determine
    // exactly how many bytes were written
    QByteArray header;

    // Append the status line
    header.append("HTTP/1.0 ");
    header.append(QByteArray::number(d->responseStatusCode) + " " + d->responseStatusReason);
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

    d->writeState = QHttpSocketPrivate::WriteHeaders;
    d->responseHeaderRemaining = header.length();

    // Write the header
    d->socket->write(header);
}

void QHttpSocket::writeRedirect(const QByteArray &path, bool permanent)
{
    setStatusCode(permanent ? MovedPermanently : Found);
    setHeader("Location", path);
    writeHeaders();
    close();
}

void QHttpSocket::writeError(int statusCode, const QByteArray &statusReason)
{
    setStatusCode(statusCode, statusReason);

    // Build the template that will be sent to the client
    QByteArray data = ErrorTemplate
            .arg(d->responseStatusCode)
            .arg(d->responseStatusReason.constData())
            .arg(QHTTPENGINE_VERSION)
            .toUtf8();

    setHeader("Content-Length", QByteArray::number(data.length()));
    setHeader("Content-Type", "text/html");

    writeHeaders();
    write(data);
    close();
}

qint64 QHttpSocket::readData(char *data, qint64 maxlen)
{
    // Ensure the connection is in the correct state for reading data
    if(d->readState == QHttpSocketPrivate::ReadHeaders) {
        return 0;
    }

    // Ensure that no more than the requested amount or the size of the buffer is read
    qint64 size = qMin(static_cast<qint64>(d->readBuffer.size()), maxlen);
    memcpy(data, d->readBuffer.constData(), size);

    // Remove the amount that was read from the buffer
    d->readBuffer.remove(0, size);
    d->requestDataRead += size;

    return size;
}

qint64 QHttpSocket::writeData(const char *data, qint64 len)
{
    // If the response headers have not yet been written, they must
    // immediately be written before the data can be
    if(d->writeState == QHttpSocketPrivate::WriteNone) {
        writeHeaders();
    }

    return d->socket->write(data, len);
}
