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

#ifndef QHTTPENGINE_QHTTPSOCKET_H
#define QHTTPENGINE_QHTTPSOCKET_H

#include <QTcpSocket>

#include "qhttpengine.h"
#include "qhttpparser.h"

class QHTTPENGINE_EXPORT QHttpSocketPrivate;

/**
 * @brief Implementation of the HTTP protocol
 * @headerfile qhttpsocket.h QHttpEngine/QHttpSocket
 *
 * QHttpSocket provides a class derived from QIODevice that can be used to
 * read data from and write data to an HTTP client through a QTcpSocket
 * provided in the constructor. The QHttpSocket will assume ownership of the
 * socket and ensure it is properly deleted. Consequently, the QTcpSocket must
 * have been allocated on the heap:
 *
 * @code
 * QTcpSocket *tcpSock = new QTcpSocket;
 * tcpSock->connectToHost(...);
 * tcpSock->waitForConnected();
 *
 * QHttpSocket *httpSock = new QHttpSocket(tcpSock);
 * @endcode
 *
 * Once the headersParsed() signal is emitted, information about the request
 * can be retrieved using the appropriate methods. As data is received, the
 * readyRead() signal is emitted and any available data can be read using
 * QIODevice::read():
 *
 * @code
 * QByteArray data;
 * connect(httpSock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
 *
 * void MyClass::onReadyRead()
 * {
 *     data.append(httpSock->readAll());
 * }
 * @endcode
 *
 * If the client sets the `Content-Length` header, the readChannelFinished()
 * signal will be emitted when the specified amount of data is read from the
 * client. Otherwise the readChannelFinished() signal will be emitted
 * immediately after the headers are read.
 *
 * The status code and headers may be set as long as no data has been written
 * to the device and the writeHeaders() method has not been called. The
 * headers are written either when the writeHeaders() method is called or when
 * data is first written to the device:
 *
 * @code
 * httpSock->setStatusCode(QHttpSocket::OK);
 * httpSock->setHeader("Content-Length", 13);
 * httpSock->write("Hello, world!");
 * @endcode
 *
 * This class also provides methods that simplify writing a redirect or an
 * HTTP error to the socket. To write a redirect, simply pass a path to the
 * writeRedirect() method. To write an error, simply pass the desired HTTP
 * status code to the writeError() method. Both methods will close the socket
 * once the response is written.
 */
class QHTTPENGINE_EXPORT QHttpSocket : public QIODevice
{
    Q_OBJECT

public:

    /**
     * Predefined constants for HTTP status codes
     */
    enum {
        /// Request was successful
        OK = 200,
        /// Resource has moved permanently
        MovedPermanently = 301,
        /// Resource is available at an alternate URI
        Found = 302,
        /// Bad client request
        BadRequest = 400,
        /// Access to the resource is forbidden
        Forbidden = 403,
        /// Resource was not found
        NotFound = 404,
        /// Method is not valid for the resource
        MethodNotAllowed = 405,
        /// An internal server error occurred
        InternalServerError = 500
    };

    /**
     * @brief Create a new QHttpSocket from a QTcpSocket
     *
     * This instance will assume ownership of the socket. That is, it will
     * make itself the parent of the socket.
     */
    QHttpSocket(QTcpSocket *socket, QObject *parent = 0);

    /**
     * @brief Retrieve the number of bytes available for reading
     *
     * This method indicates the number of bytes that could immediately be
     * read by a call to QIODevice::readAll().
     */
    virtual qint64 bytesAvailable() const;

    /**
     * @brief Determine if the device is sequential
     *
     * This method will always return true.
     */
    virtual bool isSequential() const;

    /**
     * @brief Close the device and underlying socket
     *
     * Invoking this method signifies that no more data will be written to the
     * device. It will also close the underlying QTcpSocket.
     */
    virtual void close();

    /**
     * @brief Retrieve the request method
     *
     * This method may only be called after the request headers have been
     * parsed.
     */
    QByteArray method() const;

    /**
     * @brief Retrieve the request path
     *
     * This method may only be called after the request headers have been
     * parsed.
     */
    QByteArray path() const;

    /**
     * @brief Determine if the request headers have been parsed yet
     */
    bool isHeadersParsed() const;

    /**
     * @brief Retrieve a map of request headers
     *
     * This method may only be called after the request headers have been
     * parsed. The original case of the headers is preserved but comparisons
     * are performed in a case-insensitive manner.
     */
    QHttpHeaderMap headers() const;

    /**
     * @brief Retrieve the length of the content
     *
     * This value is provided by the `Content-Length` HTTP header (if present)
     * and returns -1 if the value is not available.
     */
    qint64 contentLength() const;

    /**
     * @brief Set the response code
     *
     * This method may only be called before the response headers are written.
     *
     * The statusReason parameter may be omitted if one of the predefined
     * status code constants is used. If no response status code is explicitly
     * set, it will assume a default value of "200 OK".
     */
    void setStatusCode(int statusCode, const QByteArray &statusReason = QByteArray());

    /**
     * @brief Set a response header to a specific value
     *
     * This method may only be called before the response headers are written.
     * If the specified header already has a value set, it will be
     * overwritten.
     */
    void setHeader(const QByteArray &name, const QByteArray &value);

    /**
     * @brief Set the response headers
     *
     * This method may only be called before the response headers are written.
     * All existing headers will be overwritten.
     */
    void setHeaders(const QHttpHeaderMap &headers);

    /**
     * @brief Write response headers to the socket
     *
     * This method should not be invoked after the response headers have been
     * written.
     */
    void writeHeaders();

    /**
     * @brief Write an HTTP 3xx redirect to the socket
     */
    void writeRedirect(const QByteArray &path, bool permanent = false);

    /**
     * @brief Write an HTTP error to the socket
     */
    void writeError(int statusCode, const QByteArray &statusReason = QByteArray());

Q_SIGNALS:

    /**
     * @brief Indicate that request headers have been parsed
     *
     * This signal is emitted when the request headers have been received from
     * the client and parsing is complete. It is then safe to begin reading
     * request data. The readyRead() signal will be emitted as request data is
     * received.
     */
    void headersParsed();

protected:

    /**
     * @brief Implementation of QIODevice::readData()
     */
    virtual qint64 readData(char *data, qint64 maxlen);

    /**
     * @brief Implementation of QIODevice::writeData()
     */
    virtual qint64 writeData(const char *data, qint64 len);

private:

    QHttpSocketPrivate *const d;
    friend class QHttpSocketPrivate;
};

#endif // QHTTPENGINE_QHTTPSOCKET_H
