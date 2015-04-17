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
#include <QTcpSocket>

#include "config.h"

class QHTTPENGINE_EXPORT QHttpSocketPrivate;

/**
 * @brief Socket for communicating with a client
 *
 * QHttpSocket provides a class derived from QIODevice for communication with
 * an HTTP client. A QTcpSocket instance is provided to the constructor.
 *
 * Once the requestHeadersReadChanged() signal is emitted, information about
 * the request can be retrieved using the appropriate properties. This
 * includes the request method, URI, and headers. As request data is received,
 * the readyRead() signal is emitted and any available data can be read using
 * QIODevice's read* methods.
 *
 * Response code and headers can be set as long as no data has been written
 * to the socket. Response data can be written using QIODevice's write*
 * methods. The socket may be closed using the close() method once writing is
 * complete.
 *
 * If at any point during the exchange a protocol error is encountered, the
 * errorChanged() signal is emitted. A human-readable description of the error
 * can be obtained through the errorString() method.
 */
class QHTTPENGINE_EXPORT QHttpSocket : public QIODevice
{
    Q_OBJECT
    Q_PROPERTY(HttpError httpError READ httpError NOTIFY httpErrorChanged)
    Q_PROPERTY(QString requestMethod READ requestMethod)
    Q_PROPERTY(QString requestUri READ requestUri)
    Q_PROPERTY(QStringList requestHeaders READ requestHeaders)
    Q_PROPERTY(bool requestHeadersRead READ requestHeadersRead NOTIFY requestHeadersReadChanged)
    Q_ENUMS(HttpError)

public:

    /**
     * @brief HTTP error encountered during client communication
     */
    enum HttpError {
        NoError = 0,
        MalformedRequestLine,
        MalformedRequestHeader,
        InvalidHttpVersion
    };

    /**
     * @brief Create a new QHttpSocket from a socket
     *
     * It is assumed that the socket is already in the connected state. The
     * QHttpSocket assumes ownership of the socket.
     */
    QHttpSocket(QTcpSocket *socket, QObject *parent = 0);

    /**
     * @brief Destroy the QHttpSocket instance
     */
    virtual ~QHttpSocket();

    /**
     * @brief Close the socket
     *
     * This will cause the response headers to be written to the socket if
     * they have not yet been written.
     */
    virtual void close();

    /**
     * @brief Retrieve the last error
     */
    HttpError httpError() const;

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
     * @brief Determine if request headers have been read yet
     */
    bool requestHeadersRead() const;

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
    Q_INVOKABLE void setResponseStatusCode(const QString &statusCode);

    /**
     * @brief Set a response header to a specific value
     *
     * This method may only be called before the response headers are written.
     */
    Q_INVOKABLE void setResponseHeader(const QString &header, const QString &value);

Q_SIGNALS:

    /**
     * @brief Indicate that an HTTP error has occurred
     *
     * Any attempts to read from or write to the socket after this point will
     * be ignored. A brief description of the error condition can be retrieved
     * with the errorString() method.
     */
    void httpErrorChanged(HttpError httpError);

    /**
     * @brief Indicate that request headers have been read
     *
     * Once this signal is emitted, it is safe to begin reading request data.
     * The readyRead() signal will be emitted as request data is received.
     */
    void requestHeadersReadChanged(bool requestHeadersRead);

private:

    virtual bool isSequential() const;
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QHttpSocketPrivate *const d;
    friend class QHttpSocketPrivate;
};

#endif // QHTTPENGINE_QHTTPSOCKET_H
