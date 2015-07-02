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

#include "qhttpserver.h"
#include "qhttpserver_p.h"
#include "qhttpsocket.h"

QHttpServerPrivate::QHttpServerPrivate(QHttpServer *httpServer, QHttpHandler *httpHandler)
    : QObject(httpServer),
      q(httpServer),
      handler(httpHandler)
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(onIncomingConnection()));
}

void QHttpServerPrivate::onIncomingConnection()
{
    // Obtain the next pending connection and create a QHttpSocket from it
    QHttpSocket *socket = new QHttpSocket(server.nextPendingConnection(), this);

    // Wait until the socket finishes reading the HTTP headers to continue
    connect(socket, SIGNAL(headersParsed()), this, SLOT(onHeadersParsed()));
}

void QHttpServerPrivate::onHeadersParsed()
{
    // Obtain the socket that corresponds with the sender of the signal
    QHttpSocket *socket = qobject_cast<QHttpSocket*>(sender());

    // Obtain the path and pass it along to the handler
    handler->process(socket, QString(socket->path().mid(1)));
}

QHttpServer::QHttpServer(QHttpHandler *handler, QObject *parent)
    : QObject(parent),
      d(new QHttpServerPrivate(this, handler))
{
}

bool QHttpServer::listen(const QHostAddress &address, quint16 port)
{
    return d->server.listen(address, port);
}

QHostAddress QHttpServer::address() const
{
    return d->server.serverAddress();
}

quint16 QHttpServer::port() const
{
    return d->server.serverPort();
}
