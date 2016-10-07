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

#ifndef QHTTPENGINE_QOBJECTHANDLER_H
#define QHTTPENGINE_QOBJECTHANDLER_H

#include <QHttpEngine/QHttpHandler>

#include "qhttpengine_global.h"

class QHttpSocket;
class QHTTPENGINE_EXPORT QObjectHandlerPrivate;

/**
 * @brief Handler for invoking slots
 * @headerfile qobjecthandler.h QHttpEngine/QObjectHandler
 *
 * This handler enables incoming requests to invoke a matching slot in a
 * QObject-derived class. The slot name is used to determine the HTTP verb
 * that the method expects. For all requests, the query string is parsed and
 * supplied to the slot as a parameter. For POST requests, the request body is
 * expected to be a JSON object and it is supplied to the slot as a
 * QVariantMap. The slot is expected to return a QVariantMap containing the
 * response, which will be encoded as a JSON object.
 *
 * To use this class, it must be subclassed and one or more slots must be
 * created. The name of the slot will be used to determine the HTTP method and
 * path. For example, the following handler consists of two methods that can
 * be invoked by using the `/something` path.
 *
 * @code
 * class TestHandler : public QObjectHandler
 * {
 *     Q_OBJECT
 * private slots:
 *     QVariantMap get_something(QVariantMap queryString);
 *     QVariantMap post_something(QVariantMap queryString, QVariantMap parameters);
 * };
 * @endcode
 *
 * The slot name should begin with the HTTP method, followed by an underscore,
 * and finally the name of the method which will be used to determine the path
 * used to invoke it.
 */
class QHTTPENGINE_EXPORT QObjectHandler : public QHttpHandler
{
    Q_OBJECT

public:

    /**
     * @brief Create a new QObject handler
     */
    explicit QObjectHandler(QObject *parent = 0);

protected:

    /**
     * @brief Reimplementation of QHttpHandler::process()
     */
    virtual void process(QHttpSocket *socket, const QString &path);

    /**
     * @brief Set the status code for the current request
     *
     * By default, the status code is set to QHttpSocket::OK.
     */
    void setStatusCode(int statusCode);

private:

    QObjectHandlerPrivate *const d;
    friend class QObjectHandlerPrivate;
};

#endif // QHTTPENGINE_QOBJECTHANDLER_H
