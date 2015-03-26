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

#ifndef QHTTPENGINE_QHTTPSOCKET_H
#define QHTTPENGINE_QHTTPSOCKET_H

#include <QIODevice>
#include <QVariantMap>

#include "config.h"

class QHttpSocketPrivate;

/**
 * @brief Socket for communicating with a client
 *
 * The QHttpSocket class provides an interface for communicating with an HTTP
 * client. Method, path, and headers from the request can be obtained through
 * the appropriate properties once the requestHeadersParsed() signal is
 * emitted. The request data can be read using QIODevice's read() method.
 *
 * Response code and headers can be set through the appropriate properties.
 * The response data can be written directly using QIODevice's write() method.
 * All response headers must be set before writing any data.
 */
class QHTTPENGINE_EXPORT QHttpSocket : public QIODevice
{
    Q_OBJECT
    Q_PROPERTY(QByteArray method READ method)
    Q_PROPERTY(QByteArray path READ path)
    Q_PROPERTY(QByteArray statusCode READ statusCode WRITE setStatusCode)

public:

    /**
     * @brief Create a new QHttpSocket instance from a socket descriptor
     */
    explicit QHttpSocket(qintptr socketDescriptor, QObject *parent = 0);

    /**
     * @brief Destroy the QHttpSocket instance
     */
    virtual ~QHttpSocket();

    /**
     * @brief Retrieve the request method
     */
    QByteArray method() const;

    /**
     * @brief Retrieve the request path
     */
    QByteArray path() const;

    /**
     * @brief Retrieve the value of a request header
     */
    QByteArray requestHeader(const QByteArray &header) const;

    /**
     * @brief Retrieve the response code
     */
    QByteArray statusCode() const;

    /**
     * @brief Set the response code
     */
    void setStatusCode(const QByteArray & code);

    /**
     * @brief Set a response header to the specified value
     */
    void setResponseHeader(const QByteArray &header, const QByteArray &value);

Q_SIGNALS:

    /**
     * @brief Indicate that request headers have been parsed
     *
     * Once this signal is emitted, it is safe to begin reading request data.
     * The readyRead() signal will be emitted as request data is received.
     */
    void requestHeadersParsed();

private:

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    QHttpSocketPrivate *const d;
    friend class QHttpSocketPrivate;
};

#endif // QHTTPENGINE_QHTTPSOCKET_H
