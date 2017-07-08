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

#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include <qhttpengine/qhttpsocket.h>
#include <qhttpengine/qlocalauth.h>

#include "qlocalauth_p.h"

LocalAuthPrivate::LocalAuthPrivate(QObject *parent)
    : QObject(parent),
      tokenHeader("X-Auth-Token"),
      token(QUuid::createUuid().toString())
{
    updateFile();
}

void LocalAuthPrivate::updateFile()
{
    if (file.open()) {
        file.write(QJsonDocument(QJsonObject::fromVariantMap(data)).toJson());
        file.close();
    }
}

LocalAuth::LocalAuth(QObject *parent)
    : HttpMiddleware(parent),
      d(new LocalAuthPrivate(this))
{
}

bool LocalAuth::exists() const
{
    return d->file.exists();
}

QString LocalAuth::filename() const
{
    return d->file.fileName();
}

void LocalAuth::setData(const QVariantMap &data)
{
    d->data = data;
    d->data.insert("token", d->token);
    d->updateFile();
}

void LocalAuth::setHeaderName(const QByteArray &name)
{
    d->tokenHeader = name;
}

bool LocalAuth::process(HttpSocket *socket)
{
    if (socket->headers().value(d->tokenHeader) != d->token) {
        socket->writeError(HttpSocket::Forbidden);
        return false;
    }

    return true;
}
