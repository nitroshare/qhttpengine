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

#include <QJsonDocument>
#include <QJsonParseError>
#include <QTcpSocket>

#include <qhttpengine/parser.h>

#include "socket_p.h"

// Predefined error response requires a simple HTML template to be returned to
// the client describing the error condition
const QString ErrorTemplate =
        "<!DOCTYPE html>"
        "<html>"
          "<head>"
            "<meta charset=\"utf-8\">"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
            "<title>%1 %2</title>"
          "</head>"
          "<body>"
            "<h1>%1 %2</h1>"
            "<p>"
              "An error has occurred while trying to display the requested resource. "
              "Please contact the website owner if this error persists."
            "</p>"
            "<hr>"
            "<p><em>QHttpEngine %3</em></p>"
          "</body>"
        "</html>";

HttpSocketPrivate::HttpSocketPrivate(HttpSocket *httpSocket, QTcpSocket *tcpSocket)
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

    connect(socket, &QTcpSocket::readyRead, this, &HttpSocketPrivate::onReadyRead);
    connect(socket, &QTcpSocket::bytesWritten, this, &HttpSocketPrivate::onBytesWritten);
    connect(socket, &QTcpSocket::readChannelFinished, q, &HttpSocket::readChannelFinished);

    // Process anything already received by the socket
    onReadyRead();
}

QByteArray HttpSocketPrivate::statusReason(int statusCode) const
{
    switch (statusCode) {
    case HttpSocket::OK: return "OK";
    case HttpSocket::Created: return "CREATED";
    case HttpSocket::Accepted: return "ACCEPTED";
    case HttpSocket::PartialContent: return "PARTIAL CONTENT";
    case HttpSocket::MovedPermanently: return "MOVED PERMANENTLY";
    case HttpSocket::Found: return "FOUND";
    case HttpSocket::BadRequest: return "BAD REQUEST";
    case HttpSocket::Unauthorized: return "UNAUTHORIZED";
    case HttpSocket::Forbidden: return "FORBIDDEN";
    case HttpSocket::NotFound: return "NOT FOUND";
    case HttpSocket::MethodNotAllowed: return "METHOD NOT ALLOWED";
    case HttpSocket::Conflict: return "CONFLICT";
    case HttpSocket::BadGateway: return "BAD GATEWAY";
    case HttpSocket::ServiceUnavailable: return "SERVICE UNAVAILABLE";
    case HttpSocket::InternalServerError: return "INTERNAL SERVER ERROR";
    case HttpSocket::HttpVersionNotSupported: return "HTTP VERSION NOT SUPPORTED";
    default: return "UNKNOWN ERROR";
    }
}

void HttpSocketPrivate::onReadyRead()
{
    // Append all of the new data to the read buffer
    readBuffer.append(socket->readAll());

    // If reading headers, return if they could not be read (yet)
    if (readState == ReadHeaders && !readHeaders()) {
        return;
    }

    // Read data if in that state, otherwise discard
    switch (readState) {
    case ReadData:
        readData();
        break;
    case ReadFinished:
        readBuffer.clear();
        break;
    }
}

void HttpSocketPrivate::onBytesWritten(qint64 bytes)
{
    // Check to see if all of the response header was written
    if (writeState == WriteHeaders) {
        if (responseHeaderRemaining - bytes > 0) {
            responseHeaderRemaining -= bytes;
        } else {
            writeState = WriteData;
            bytes -= responseHeaderRemaining;
        }
    }

    // Only emit bytesWritten() for data after the headers
    if (writeState == WriteData) {
        Q_EMIT q->bytesWritten(bytes);
    }
}

bool HttpSocketPrivate::readHeaders()
{
    // Check for the double CRLF that signals the end of the headers and
    // if it is not found, wait until the next time readyRead is emitted
    int index = readBuffer.indexOf("\r\n\r\n");
    if (index == -1) {
        return false;
    }

    // Attempt to parse the headers and if a problem is encountered, abort
    // the connection (so that no more data is read or written) and return
    if (!HttpParser::parseRequestHeaders(readBuffer.left(index), requestMethod, requestRawPath, requestHeaders) ||
            !HttpParser::parsePath(requestRawPath, requestPath, requestQueryString)) {
        q->writeError(HttpSocket::BadRequest);
        return false;
    }

    // Remove the headers from the buffer
    readBuffer.remove(0, index + 4);
    readState = ReadData;

    // If the content-length header is present, use it to determine
    // how much data to expect from the socket - not all requests
    // use this header - WebSocket requests, for example, do not
    if (requestHeaders.contains("Content-Length")) {
        requestDataTotal = requestHeaders.value("Content-Length").toLongLong();
    }

    // Indicate that the headers have been parsed
    Q_EMIT q->headersParsed();

    return true;
}

void HttpSocketPrivate::readData()
{
    // Emit the readyRead() signal if any data is available in the buffer
    if (readBuffer.size()) {
        Q_EMIT q->readyRead();
    }

    // Check to see if the specified amount of data has been read from the
    // socket, if so, emit the readChannelFinished() signal
    if (requestDataTotal != -1 &&
            requestDataRead + readBuffer.size() >= requestDataTotal) {
        readState = ReadFinished;
        Q_EMIT q->readChannelFinished();
    }
}

HttpSocket::HttpSocket(QTcpSocket *socket, QObject *parent)
    : QIODevice(parent),
      d(new HttpSocketPrivate(this, socket))
{
    // The device is initially open for both reading and writing
    setOpenMode(QIODevice::ReadWrite);
}

qint64 HttpSocket::bytesAvailable() const
{
    if (d->readState > HttpSocketPrivate::ReadHeaders) {
        return d->readBuffer.size() + QIODevice::bytesAvailable();
    } else {
        return 0;
    }
}

bool HttpSocket::isSequential() const
{
    return true;
}

void HttpSocket::close()
{
    // Invoke the parent method
    QIODevice::close();

    d->readState = HttpSocketPrivate::ReadFinished;
    d->writeState = HttpSocketPrivate::WriteFinished;

    d->socket->close();
}

QHostAddress HttpSocket::peerAddress() const
{
    return d->socket->peerAddress();
}

bool HttpSocket::isHeadersParsed() const
{
    return d->readState > HttpSocketPrivate::ReadHeaders;
}

HttpSocket::Method HttpSocket::method() const
{
    return d->requestMethod;
}

QByteArray HttpSocket::rawPath() const
{
    return d->requestRawPath;
}

QString HttpSocket::path() const
{
    return d->requestPath;
}

HttpSocket::QueryStringMap HttpSocket::queryString() const
{
    return d->requestQueryString;
}

HttpSocket::HeaderMap HttpSocket::headers() const
{
    return d->requestHeaders;
}

qint64 HttpSocket::contentLength() const
{
    return d->requestDataTotal;
}

bool HttpSocket::readJson(QJsonDocument &document)
{
    QJsonParseError error;
    document = QJsonDocument::fromJson(readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        writeError(HttpSocket::BadRequest);
        return false;
    }

    return true;
}

void HttpSocket::setStatusCode(int statusCode, const QByteArray &statusReason)
{
    d->responseStatusCode = statusCode;
    d->responseStatusReason = statusReason.isNull() ? d->statusReason(statusCode) : statusReason;
}

void HttpSocket::setHeader(const QByteArray &name, const QByteArray &value, bool replace)
{
    if (replace || d->responseHeaders.count(name)) {
        d->responseHeaders.replace(name, value);
    } else {
        d->responseHeaders.replace(name, d->responseHeaders.value(name) + ", " + value);
    }
}

void HttpSocket::setHeaders(const HeaderMap &headers)
{
    d->responseHeaders = headers;
}

void HttpSocket::writeHeaders()
{
    // Use a QByteArray for building the header so that we can later determine
    // exactly how many bytes were written
    QByteArray header;

    // Append the status line
    header.append("HTTP/1.0 ");
    header.append(QByteArray::number(d->responseStatusCode) + " " + d->responseStatusReason);
    header.append("\r\n");

    // Append each of the headers followed by a CRLF
    for (auto i = d->responseHeaders.constBegin(); i != d->responseHeaders.constEnd(); ++i) {
        header.append(i.key());
        header.append(": ");
        header.append(d->responseHeaders.values(i.key()).join(", "));
        header.append("\r\n");
    }

    // Append an extra CRLF
    header.append("\r\n");

    d->writeState = HttpSocketPrivate::WriteHeaders;
    d->responseHeaderRemaining = header.length();

    // Write the header
    d->socket->write(header);
}

void HttpSocket::writeRedirect(const QByteArray &path, bool permanent)
{
    setStatusCode(permanent ? MovedPermanently : Found);
    setHeader("Location", path);
    writeHeaders();
    close();
}

void HttpSocket::writeError(int statusCode, const QByteArray &statusReason)
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

void HttpSocket::writeJson(const QJsonDocument &document, int statusCode)
{
    QByteArray data = document.toJson();
    setStatusCode(statusCode);
    setHeader("Content-Length", QByteArray::number(data.length()));
    setHeader("Content-Type", "application/json");
    write(data);
    close();
}

qint64 HttpSocket::readData(char *data, qint64 maxlen)
{
    // Ensure the connection is in the correct state for reading data
    if (d->readState == HttpSocketPrivate::ReadHeaders) {
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

qint64 HttpSocket::writeData(const char *data, qint64 len)
{
    // If the response headers have not yet been written, they must
    // immediately be written before the data can be
    if (d->writeState == HttpSocketPrivate::WriteNone) {
        writeHeaders();
    }

    return d->socket->write(data, len);
}
