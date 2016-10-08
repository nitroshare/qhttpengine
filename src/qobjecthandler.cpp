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

#include <QGenericArgument>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaMethod>

#include <QHttpEngine/QObjectHandler>

#include "qobjecthandler_p.h"

QObjectHandlerPrivate::QObjectHandlerPrivate(QObjectHandler *handler)
    : QObject(handler),
      q(handler)
{
}

void QObjectHandlerPrivate::invokeSlot(QHttpSocket *socket, const QString &path)
{
    Method m = map.value(path);
    QVariantMap parameters;

    // If data was supplied, decode it as JSON
    if (socket->bytesAvailable()) {
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(socket->readAll(), &error);

        // Ensure that the document is valid
        if (error.error != QJsonParseError::NoError) {
            socket->writeError(QHttpSocket::BadRequest);
            return;
        }

        parameters = document.object().toVariantMap();
    }

    QVariantMap retVal;

    // Invoke the slot
    if (m.oldSlot) {

        // Obtain the slot index
        int index = m.receiver->metaObject()->indexOfSlot(m.slot.method + 1);
        if (index == -1) {
            socket->writeError(QHttpSocket::InternalServerError);
            return;
        }

        QMetaMethod method = m.receiver->metaObject()->method(index);

        // Ensure the parameters are correct
        QList<QByteArray> params = method.parameterTypes();
        if (params.count() > 0 && params.at(0) != "QHttpSocket*" ||
                params.count() > 1 && params.at(1) != "QVariantMap" ||
                params.count() > 2 ||
                method.returnType() != QMetaType::QVariantMap) {
            socket->writeError(QHttpSocket::InternalServerError);
            return;
        }

        // Invoke the method
        if (!m.receiver->metaObject()->method(index).invoke(
                    m.receiver,
                    Q_RETURN_ARG(QVariantMap, retVal),
                    Q_ARG(QHttpSocket*, socket),
                    Q_ARG(QVariantMap, parameters))) {
            socket->writeError(QHttpSocket::InternalServerError);
            return;
        }
    } else {
        void *args[3] = {
            &retVal,
            &socket,
            &parameters
        };
        m.slot.slotObj->call(m.receiver, args);
    }

    // Convert the return value to JSON and write it to the socket
    QByteArray data = QJsonDocument(QJsonObject::fromVariantMap(retVal)).toJson();
    socket->setHeader("Content-Length", QByteArray::number(data.length()));
    socket->setHeader("Content-Type", "application/json");
    socket->write(data);
    socket->close();
}

QObjectHandler::QObjectHandler(QObject *parent)
    : QHttpHandler(parent),
      d(new QObjectHandlerPrivate(this))
{
}

void QObjectHandler::process(QHttpSocket *socket, const QString &path)
{
    // Ensure the method has been registered
    if (!d->map.contains(path)) {
        socket->writeError(QHttpSocket::NotFound);
        return;
    }

    // Ensure the method is accepted
    QObjectHandlerPrivate::Method m = d->map.value(path);
    if (!(m.acceptedMethods & socket->method())) {
        // TODO: accept header
        socket->writeError(QHttpSocket::MethodNotAllowed);
        return;
    }

    // If the slot has finished receiving all of the data, jump directly to
    // invokeSlot(), otherwise, wait until we have the rest of it
    if (socket->bytesAvailable() >= socket->contentLength()) {
        d->invokeSlot(socket, path);
    } else {
        connect(socket, &QHttpSocket::readChannelFinished, [this, socket, path]() {
            d->invokeSlot(socket, path);
        });
    }
}

void QObjectHandler::registerMethod(const QString &name, QObject *receiver, const char *method, int acceptedStatusCodes)
{
    d->map.insert(name, QObjectHandlerPrivate::Method(receiver, method, acceptedStatusCodes));
}

void QObjectHandler::registerMethodImpl(const QString &name, QObject *receiver, QtPrivate::QSlotObjectBase *slotObj, int acceptedStatusCodes)
{
    d->map.insert(name, QObjectHandlerPrivate::Method(receiver, slotObj, acceptedStatusCodes));
}
