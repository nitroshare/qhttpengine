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

#ifndef QHTTPENGINE_QHTTPHANDLER_H
#define QHTTPENGINE_QHTTPHANDLER_H

#include <QObject>

#include "config.h"
#include "qhttpsocket.h"

/**
 * @class QHttpHandler qhttphandler.h QHttpHandler
 * @brief Base class for URL handlers
 *
 * When a request is received by a QHttpServer, it is dispatched to a
 * QHttpHandler instance which will then determine what happens to the
 * request. This is done by reimplementing the process() method.
 */
class QHTTPENGINE_EXPORT QHttpHandler : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Base constructor for a handler
     *
     * Because this class is abstract, it cannot be instantiated.
     */
    explicit QHttpHandler(QObject *parent = 0);

    /**
     * @brief Attempt to process a request
     *
     * This method should attempt to process the request if the provided path
     * matches a resource. If the request could not be processed, this method
     * should return false and a 404 page will be served.
     *
     * Note that the leading "/" will be stripped from the path.
     */
    virtual bool process(QHttpSocket *socket, const QString &path) = 0;
};

#endif // QHTTPENGINE_QHTTPHANDLER_H
