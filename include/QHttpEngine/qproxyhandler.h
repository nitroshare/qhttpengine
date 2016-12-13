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

#ifndef QHTTPENGINE_QPROXYHANDLER_H
#define QHTTPENGINE_QPROXYHANDLER_H

#include <QHostAddress>

#include <QHttpEngine/QHttpHandler>

#include "qhttpengine_global.h"

class QHTTPENGINE_EXPORT QProxyHandlerPrivate;

/**
 * @brief Handler that routes HTTP requests to an upstream server
 */
class QHTTPENGINE_EXPORT QProxyHandler : public QHttpHandler
{
    Q_OBJECT

public:

    /**
     * @brief Create a new proxy handler
     */
    QProxyHandler(const QHostAddress &address, quint16 port, QObject *parent = 0);

protected:

    /**
     * @brief Reimplementation of QHttpHandler::process()
     */
    virtual void process(QHttpSocket *socket, const QString &path);

private:

    QProxyHandlerPrivate *const d;
};

#endif // QHTTPENGINE_QPROXYHANDLER_H
