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
#include <QStringList>

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
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString requestMethod READ requestMethod)
    Q_PROPERTY(QString requestUri READ requestUri)
    Q_PROPERTY(QStringList requestHeaders READ requestHeaders)
    Q_ENUMS(Error)

public:

    /**
     * @brief Error encountered during client communication
     */
    enum Error {
        None = 0,
        MalformedRequestLine,
        MalformedRequestHeader,
        InvalidHttpVersion,
        IncompleteHeader
    };

    /**
     * @brief Create a new QHttpSocket instance from a socket descriptor
     */
    QHttpSocket(qintptr socketDescriptor, QObject *parent = 0);

    /**
     * @brief Destroy the QHttpSocket instance
     */
    virtual ~QHttpSocket();

    /**
     * @brief Retrieve the last error
     */
    Error error() const;

    /**
     * @brief Retrieve the request method
     *
     * This method may only be called after the requestHeadersParsed() signal
     * is emitted.
     */
    QString requestMethod() const;

    /**
     * @brief Retrieve the request URI
     *
     * This method may only be called after the requestHeadersParsed() signal
     * is emitted.
     */
    QString requestUri() const;

    /**
     * @brief Retrieve all request headers
     *
     * This method may only be called after the requestHeadersParsed() signal
     * is emitted.
     */
    QStringList requestHeaders() const;

    /**
     * @brief Retrieve the value of a specific request header
     *
     * This method may only be called after the requestHeadersParsed() signal
     * is emitted. Headers are case-insensitive. If the specified header was
     * not set, then a null (empty) string is returned.
     */
    Q_INVOKABLE QString requestHeader(const QString &header) const;

    /**
     * @brief Set the response code
     *
     * This method may only be called before the response headers are written.
     * If no response status code is explicitly set, it will assume a default
     * value of "200 OK".
     */
    void setResponseStatusCode(const QString &statusCode);

    /**
     * @brief Set a response header to a specific value
     *
     * This method may only be called before the response headers are written.
     */
    Q_INVOKABLE void setResponseHeader(const QString &header, const QString &value);

Q_SIGNALS:

    /**
     * @brief Indicate that an error has occurred
     *
     * Any attempts to read from or write to the socket after this point will
     * be ignored. A brief description of the error condition can be retrieved
     * with the errorString() method.
     */
    void errorChanged(Error error);

    /**
     * @brief Indicate that request headers have been parsed
     *
     * Once this signal is emitted, it is safe to begin reading request data.
     * The readyRead() signal will be emitted as request data is received.
     */
    void requestHeadersParsed();

private:

    virtual bool isSequential() const;
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QHttpSocketPrivate *const d;
    friend class QHttpSocketPrivate;
};

#endif // QHTTPENGINE_QHTTPSOCKET_H
