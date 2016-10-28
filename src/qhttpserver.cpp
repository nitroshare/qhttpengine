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

#if !defined(QT_NO_SSL)
#  include <QSslSocket>
#endif

#include <QHttpEngine/QHttpHandler>
#include <QHttpEngine/QHttpSocket>

#include "qhttpserver_p.h"

QHttpServerPrivate::QHttpServerPrivate(QHttpServer *httpServer)
    : QObject(httpServer),
      q(httpServer),
      handler(0)
{
}

void QHttpServerPrivate::process(QTcpSocket *socket)
{
    QHttpSocket *httpSocket = new QHttpSocket(socket, this);

    // Wait until the socket finishes reading the HTTP headers before routing
    connect(httpSocket, &QHttpSocket::headersParsed, [this, httpSocket]() {
        if (handler) {
            handler->route(httpSocket, QString(httpSocket->path().mid(1)));
        } else {
            httpSocket->writeError(QHttpSocket::InternalServerError);
        }
    });

    // Destroy the socket once the client is disconnected
    connect(socket, &QTcpSocket::disconnected, httpSocket, &QHttpSocket::deleteLater);
}

QHttpServer::QHttpServer(QObject *parent)
    : QTcpServer(parent),
      d(new QHttpServerPrivate(this))
{
}

QHttpServer::QHttpServer(QHttpHandler *handler, QObject *parent)
    : QTcpServer(parent),
      d(new QHttpServerPrivate(this))
{
    setHandler(handler);
}

void QHttpServer::setHandler(QHttpHandler *handler)
{
    d->handler = handler;
}

#if !defined(QT_NO_SSL)
void QHttpServer::setSslConfiguration(const QSslConfiguration &configuration)
{
    d->configuration = configuration;
}
#endif

void QHttpServer::incomingConnection(qintptr socketDescriptor)
{
#if !defined(QT_NO_SSL)
    if (!d->configuration.isNull()) {

        // Initialize the socket with the SSL configuration
        QSslSocket *socket = new QSslSocket(this);

        // Wait until encryption is complete before processing the socket
        connect(socket, &QSslSocket::encrypted, [this, socket]() {
            d->process(socket);
        });

        socket->setSocketDescriptor(socketDescriptor);
        socket->setSslConfiguration(d->configuration);
        socket->startServerEncryption();

    } else {
#endif

        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);

        // Process the socket immediately
        d->process(socket);

#if !defined(QT_NO_SSL)
    }
#endif
}
