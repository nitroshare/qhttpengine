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

#ifndef QHTTPENGINE_QHTTPRESPONSE_H
#define QHTTPENGINE_QHTTPRESPONSE_H

#include <QIODevice>

#include "config.h"

class QHTTPENGINE_EXPORT QHttpResponsePrivate;

/**
 * @brief HTTP response generator
 */
class QHTTPENGINE_EXPORT QHttpResponse : public QIODevice
{
    Q_OBJECT

public:

    /**
     * @brief Create a new QHttpResponse from a QIODevice
     *
     * It is assumed that the device is already opened for writing
     */
    QHttpResponse(QIODevice *device, QObject *parent = 0);

    /**
     * @brief Destroy the QHttpResponse
     */
    virtual ~QHttpResponse();

    /**
     * @brief Determine if the device is sequential
     *
     * This method will always return true.
     */
    virtual bool isSequential() const;

    /**
     * @brief Write response headers to the device
     */
    Q_INVOKABLE void writeHeaders();

    /**
     * @brief Set the response code
     *
     * This method may only be called before the response headers are written.
     * If no response status code is explicitly set, it will assume a default
     * value of "200 OK".
     */
    Q_INVOKABLE void setStatusCode(const QByteArray &statusCode);

    /**
     * @brief Set a response header to a specific value
     *
     * This method may only be called before the response headers are written.
     */
    Q_INVOKABLE void setHeader(const QByteArray &name, const QByteArray &value);

private:

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QHttpResponsePrivate *const d;
    friend class QHttpResponsePrivate;
};

#endif // QHTTPENGINE_QHTTPRESPONSE_H
