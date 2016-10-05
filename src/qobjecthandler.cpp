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
#include <QJsonParseError>
#include <QList>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaType>
#include <QPair>
#include <QUrl>
#include <QUrlQuery>
#include <QVariantMap>

#include "QHttpEngine/qobjecthandler.h"
#include "qobjecthandler_p.h"

const QString MethodGET = "GET";
const QString MethodPOST = "POST";

QObjectHandlerPrivate::QObjectHandlerPrivate(QObjectHandler *handler)
    : QObject(handler),
      q(handler)
{
}

void QObjectHandlerPrivate::invokeSlot(QHttpSocket *socket, int index, const QVariantMap &query)
{
    QGenericArgument secondArgument;
    QVariantMap parameters;

    statusCode = QHttpSocket::OK;

    // If this is a POST request, then decode the request body
    if (socket->method() == MethodPOST) {

        // Attempt to decode the JSON from the socket
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(socket->readAll(), &error);

        // Ensure that the document is valid
        if (error.error != QJsonParseError::NoError) {
            socket->writeError(QHttpSocket::BadRequest);
            return;
        }

        parameters = document.object().toVariantMap();
        secondArgument = Q_ARG(QVariantMap, parameters);
    }

    // Attempt to invoke the slot
    QVariantMap retVal;
    if (!q->metaObject()->method(index).invoke(q,
            Q_RETURN_ARG(QVariantMap, retVal),
            Q_ARG(QVariantMap, query),
            secondArgument)) {
        socket->writeError(QHttpSocket::InternalServerError);
        return;
    }

    // Convert the return value to JSON and write it to the socket
    QByteArray data = QJsonDocument(QJsonObject::fromVariantMap(retVal)).toJson();
    socket->setStatusCode(statusCode);
    socket->setHeader("Content-Length", QByteArray::number(data.length()));
    socket->setHeader("Content-Type", "application/json");
    socket->write(data);
    socket->close();
}

QVariantMap QObjectHandlerPrivate::convertQueryString(const QString &query)
{
    QVariantMap map;
    QPair<QString, QString> pair;
    foreach (pair, QUrlQuery(query).queryItems()) {
        map.insert(pair.first, pair.second);
    }
    return map;
}

QObjectHandler::QObjectHandler(QObject *parent)
    : QHttpHandler(parent),
      d(new QObjectHandlerPrivate(this))
{
}

void QObjectHandler::process(QHttpSocket *socket, const QString &path)
{
    // Determine the correct slot name to invoke based on the HTTP method and
    // the path that was provided - note that the path needs to be split to
    // remove the query string
    QUrl url(path);
    QVariantMap query = d->convertQueryString(url.query());
    QString slotName = QString("%1_%2")
        .arg(QString(socket->method().toLower()))
        .arg(url.path());

    // Determine the parameters the slot should have based on the method
    QString parameters;
    if (socket->method() == MethodGET) {
        parameters = "QVariantMap";
    } else {
        parameters = "QVariantMap,QVariantMap";
    }

    // Attempt to find the method's index so we can invoke it later
    int index = metaObject()->indexOfSlot(QString("%1(%2)").arg(slotName).arg(parameters).toUtf8().data());

    // If the index is invalid, the "resource" was not found
    // TODO: an HTTP 405 should be returned when the method is incorrect
    if (index == -1) {
        socket->writeError(QHttpSocket::NotFound);
        return;
    }

    // Ensure that the return type of the slot is QVariantMap
    QMetaMethod method = metaObject()->method(index);
    if (method.returnType() != QMetaType::QVariantMap) {
        socket->writeError(QHttpSocket::InternalServerError);
        return;
    }

    // If the slot has finished receiving all of the data, jump directly to
    // invokeSlot(), otherwise, wait until we have the rest of it
    if (socket->bytesAvailable() >= socket->contentLength()) {
        d->invokeSlot(socket, index, query);
    } else {
        connect(socket, &QHttpSocket::readChannelFinished, [this, socket, index, &query]() {
            d->invokeSlot(socket, index, query);
        });
    }
}

void QObjectHandler::setStatusCode(int statusCode)
{
    d->statusCode = statusCode;
}
