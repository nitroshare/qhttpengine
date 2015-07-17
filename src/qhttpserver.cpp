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

#include <QTcpSocket>

#include "QHttpEngine/qhttpserver.h"
#include "QHttpEngine/qhttpsocket.h"
#include "qhttpserver_p.h"

QHttpServerPrivate::QHttpServerPrivate(QHttpServer *httpServer)
    : QObject(httpServer),
      q(httpServer),
      handler(0)
{
    connect(q, SIGNAL(newConnection()), this, SLOT(onIncomingConnection()));
}

void QHttpServerPrivate::onIncomingConnection()
{
    // Obtain the next pending connection and create a QHttpSocket from it
    QTcpSocket *tcpSocket = q->nextPendingConnection();
    QHttpSocket *httpSocket = new QHttpSocket(tcpSocket, this);

    // Wait until the socket finishes reading the HTTP headers before routing
    connect(httpSocket, SIGNAL(headersParsed()), this, SLOT(onHeadersParsed()));

    // Destroy the socket once the client is disconnected
    connect(tcpSocket, SIGNAL(disconnected()), httpSocket, SLOT(deleteLater()));
}

void QHttpServerPrivate::onHeadersParsed()
{
    // Obtain the socket that corresponds with the sender of the signal
    QHttpSocket *socket = qobject_cast<QHttpSocket*>(sender());

    // Ensure that a handler has been set
    if(handler) {

        // Obtain the path, strip the initial "/", and pass it along to the handler
        handler->route(socket, QString(socket->path().mid(1)));

    } else {

        // Return an HTTP 500 error to the client
        socket->writeError(QHttpSocket::InternalServerError);
    }
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
