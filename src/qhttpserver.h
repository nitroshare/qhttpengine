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

#ifndef QHTTPENGINE_QHTTPSERVER_H
#define QHTTPENGINE_QHTTPSERVER_H

#include <QHostAddress>
#include <QObject>
#include <QTcpServer>

#include "qhttpengine.h"
#include "qhttphandler.h"

class QHTTPENGINE_EXPORT QHttpServerPrivate;

/**
 * @brief HTTP server
 * @headerfile qhttpserver.h QHttpServer
 *
 * This class provides an HTTP server capable of receiving incoming requests
 * and routing them to the appropriate handler based on their request paths.
 * The handler is provided as an argument to the constructor. The following
 * example creates a server that will respond to requests for files in the
 * /var/www directory:
 *
 * @code
 * QFileSystemHandler fshandler("/var/www");
 * QHttpServer server(&fshandler);
 * server.listen();
 *
 * // Note: use server.serverAddress() and server.serverPort() to connect
 * @endcode
 *
 * The QSubHandler class allows multiple handlers to be used with a single
 * server while using the request path to route requests to the appropriate
 * handler.
 */
class QHTTPENGINE_EXPORT QHttpServer : public QTcpServer
{
    Q_OBJECT

public:

    /**
     * @brief Create an HTTP server with the specified handler
     */
    QHttpServer(QHttpHandler *handler, QObject *parent = 0);

private:

    QHttpServerPrivate *const d;
    friend class QHttpServerPrivate;
};

#endif // QHTTPENGINE_QHTTPSERVER_H
