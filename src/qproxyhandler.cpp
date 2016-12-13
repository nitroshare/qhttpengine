/*
 * Copyright (c) 2016 Nathan Osman
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

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <QHttpEngine/QProxyHandler>

#include "qproxyhandler_p.h"

QProxyHandlerPrivate::QProxyHandlerPrivate(QObject *parent, const QHostAddress &address, quint16 port)
    : QObject(parent),
      address(address),
      port(port)
{
}

QByteArray QProxyHandlerPrivate::methodToString(QHttpSocket::Method method)
{
    switch (method) {
    case QHttpSocket::OPTIONS: return "OPTIONS";
    case QHttpSocket::GET: return "GET";
    case QHttpSocket::HEAD: return "HEAD";
    case QHttpSocket::POST: return "POST";
    case QHttpSocket::PUT: return "PUT";
    case QHttpSocket::DELETE: return "DELETE";
    case QHttpSocket::TRACE: return "TRACE";
    case QHttpSocket::CONNECT: return "CONNECT";
    default: return QByteArray();
    }
}

QProxyHandler::QProxyHandler(const QHostAddress &address, quint16 port, QObject *parent)
    : QHttpHandler(parent),
      d(new QProxyHandlerPrivate(this, address, port))
{
}

void QProxyHandler::process(QHttpSocket *socket, const QString &path)
{
    // Parent the socket to the proxy
    socket->setParent(this);

    // Build the request for the upstream server
    QUrl url(QString("http://%1:%2/%3").arg(d->address.toString()).arg(d->port).arg(path));
    QNetworkRequest request(url);

    // Copy all of the request headers
    for (auto i = socket->headers().constBegin(); i != socket->headers().constEnd(); ++i) {
        request.setRawHeader(i.key(), i.value());
    }

    // Add peer to X-Forwarded-For and set X-Real-IP if not already set
    QByteArray peerIP = socket->peerAddress().toString().toUtf8();
    QByteArray origFwd = socket->headers().value("X-Forwarded-For");
    if (origFwd.isNull()) {
        request.setRawHeader("X-Forwarded-For", peerIP);
    } else {
        request.setRawHeader("X-Forwarded-For", origFwd + ", " + peerIP);
    }
    if (!socket->headers().contains("X-Real-IP")) {
        request.setRawHeader("X-Real-IP", peerIP);
    }

    // Begin the request
    QNetworkReply *reply = d->networkAccessManager.sendCustomRequest(
        request,
        d->methodToString(socket->method()),
        socket
    );
    reply->setParent(this);

    // Copy all of the response headers when they arrive
    connect(reply, &QNetworkReply::metaDataChanged, [socket, reply]() {
        foreach (QByteArray headerName, reply->rawHeaderList()) {
            socket->setHeader(headerName, reply->rawHeader(headerName));
        }
        socket->writeHeaders();
    });

    // When data arrives from upstream, send it downstream
    connect(reply, &QNetworkReply::readyRead, [socket, reply]() {
        socket->write(reply->readAll());
    });

    // When either end disconnects, sever everything and tear it down
    connect(socket, &QHttpSocket::aboutToClose, reply, &QNetworkReply::close);
    connect(reply, &QNetworkReply::finished, socket, &QHttpSocket::close);
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}
