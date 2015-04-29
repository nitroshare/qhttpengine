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

#ifndef QHTTPENGINE_QHTTPREQUEST_H
#define QHTTPENGINE_QHTTPREQUEST_H

#include <QIODevice>
#include <QList>

#include "config.h"

class QHTTPENGINE_EXPORT QHttpRequestPrivate;

/**
 * @brief HTTP request parser
 */
class QHTTPENGINE_EXPORT QHttpRequest : public QIODevice
{
    Q_OBJECT
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QByteArray method READ method)
    Q_PROPERTY(QByteArray path READ path)
    Q_PROPERTY(bool headersParsed READ headersParsed NOTIFY headersParsedChanged)
    Q_ENUMS(Error)

public:

    /**
     * @brief Error encountered while parsing request headers
     */
    enum Error {
        NoError = 0,
        MalformedRequestLine,
        MalformedRequestHeader,
        InvalidHttpVersion
    };

    /**
     * @brief Create a new QHttpRequest from a QIODevice
     *
     * It is assumed that the device is already opened for reading.
     */
    QHttpRequest(QIODevice *device, QObject *parent = 0);

    /**
     * @brief Destroy the QHttpRequest
     */
    virtual ~QHttpRequest();

    /**
     * @brief Retrieve the number of bytes available for reading
     */
    virtual qint64 bytesAvailable() const;

    /**
     * @brief Determine if the device is sequential
     *
     * This method will always return true.
     */
    virtual bool isSequential() const;

    /**
     * @brief Retrieve the most recent error
     */
    Error error() const;

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
     * @brief Determine if request headers have been parsed yet
     */
    bool headersParsed() const;

    /**
     * @brief Retrieve a list of request headers
     *
     * This method may only be called after the request headers have been
     * parsed.
     */
    Q_INVOKABLE QList<QByteArray> headers() const;

    /**
     * @brief Retrieve the value of a specific request header
     *
     * This method may only be called after the request headers have been
     * parsed. Headers are case-insensitive. If the specified header was not
     * set, then a null byte array is returned.
     */
    Q_INVOKABLE QByteArray header(const QByteArray &name) const;

Q_SIGNALS:

    /**
     * @brief Indicate that a parsing error has occurred
     *
     * Any attempts to read from the device after this point may fail. A brief
     * description of the error condition can be retrieved with the
     * errorString() method.
     */
    void errorChanged(Error error);

    /**
     * @brief Indicate that request headers have been parsed
     *
     * Once this signal is emitted, it is safe to begin reading request data.
     * The readyRead() signal will be emitted as request data is received.
     */
    void headersParsedChanged(bool headersParsed);

private:

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QHttpRequestPrivate *const d;
    friend class QHttpRequestPrivate;
};

Q_DECLARE_METATYPE(QHttpRequest::Error)

#endif // QHTTPENGINE_QHTTPREQUEST_H
