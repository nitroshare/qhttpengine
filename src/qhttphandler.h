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

#include "qhttpengine.h"
#include "qhttpsocket.h"

class QHTTPENGINE_EXPORT QHttpHandlerPrivate;

/**
 * @brief Base class for URL handlers
 * @headerfile qhttphandler.h QHttpHandler
 *
 * When a request is received by a QHttpServer, it invokes the route() method
 * of its handler which is used to determine what happens to the request. All
 * HTTP handlers derive from this class and should override the protected
 * process() method in order to process the request.
 *
 * It is possible to attach sub-handlers to specific paths or patterns. When
 * this is done, incoming requests are first compared to the list of patterns
 * and if a match is found, that handler is invoked instead. The following
 * example creates a handler that will invoke a sub-handler when the request
 * path begins with `/test/`:
 *
 * @code
 * QHttpHandler handler, subHandler;
 * handler.addSubHandler("/test/", &subHandler);
 * @endcode
 *
 * When a request is to be processed by the handler, the process() method is
 * invoked with the following parameters:
 *
 *  - a pointer to the HTTP socket
 *  - the request path with the parts matching the pattern removed
 *
 * Using the previous example, an incoming request for `/test/123` would cause
 * the subHandler's process() method to be invoked with the path set to `123`.
 *
 * The default implementation of process() simply returns an HTTP 404 error.
 */
class QHTTPENGINE_EXPORT QHttpHandler : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Base constructor for a handler
     */
    explicit QHttpHandler(QObject *parent = 0);

    /**
     * @brief Add a handler for a specific pattern
     *
     * The pattern and handler will be added to an internal list that will be
     * used when the route() method is invoked to determine whether the
     * request matches any patterns. The order of the list is preserved.
     */
    void addSubHandler(const QRegExp &pattern, QHttpHandler *handler);

    /**
     * @brief Route an incoming request
     *
     * If the request path matches a sub-handler, it will be routed to that
     * handler. Otherwise the process() method will be invoked.
     */
    void route(QHttpSocket *socket, const QString &path);

protected:

    /**
     * @brief Process a request
     *
     * This method should process the request either by fulfilling it, sending
     * a redirect with QHttpSocket::writeRedirect(), or writing an error to
     * the socket using QHttpSocket::writeError().
     *
     * Note that the leading "/" will be stripped from the path.
     */
    virtual void process(QHttpSocket *socket, const QString &path);

private:

    QHttpHandlerPrivate *const d;
};

#endif // QHTTPENGINE_QHTTPHANDLER_H
