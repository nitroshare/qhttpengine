/**
 * The MIT License (MIT)
 *
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

#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaType>

#include "qobjecthandler.h"
#include "qobjecthandler_p.h"

QObjectHandlerPrivate::QObjectHandlerPrivate(QObjectHandler *handler)
    : QObject(handler),
      q(handler)
{
}

QObjectHandler::QObjectHandler(QObject *parent)
    : QHttpHandler(parent),
      d(new QObjectHandlerPrivate(this))
{
}

bool QObjectHandler::process(QHttpSocket *socket, const QString &path)
{
    // Determine the index of the slot with the specified name - note that we
    // don't need to worry about retrieving the index for deleteLater() since
    // we specify the "QVariant" parameter type, which no parent slots use
    int index = metaObject()->indexOfSlot(QString("%1(QVariant)").arg(path).toUtf8().data());

    // Ensure that the index is valid
    if(index == -1) {
        return false;
    }

    // Ensure that the return type is correct
    QMetaMethod method = metaObject()->method(index);
    if(method.returnType() != QMetaType::QVariant) {
        return false;
    }

    // TODO: invoke the slot as soon as the response data is read

    return true;
}
