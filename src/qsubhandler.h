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

#ifndef QHTTPENGINE_QSUBHANDLER_H
#define QHTTPENGINE_QSUBHANDLER_H

#include <QRegExp>

#include "config.h"
#include "qhttphandler.h"

class QHTTPENGINE_EXPORT QSubHandlerPrivate;

/**
 * @brief Parent handler for routing requests
 * @headerfile qsubhandler.h QSubHandler
 *
 * This handler maintains a list of patterns that map to other handlers. When
 * a request is received, the list is enumerated (in order) and if the request
 * matches one of the patterns, its handler is invoked.
 *
 * When a handler attached to this one is invoked, the portion of the pattern
 * that matches is removed from the path.
 */
class QHTTPENGINE_EXPORT QSubHandler : public QHttpHandler
{
    Q_OBJECT

public:

    /**
     * @brief Create a new sub handler
     */
    explicit QSubHandler(QObject *parent = 0);

    /**
     * @brief Reimplementation of QHttpHandler::process()
     */
    virtual bool process(QHttpSocket *socket, const QString &path);

    /**
     * @brief Add a handler for a specific pattern
     *
     * The pattern and handler will be added to an internal list that will be
     * used when the process() method is invoked to determine whether the
     * request matches any patterns. Order is preserved.
     */
    void addHandler(const QRegExp &pattern, QHttpHandler *handler);

private:

    QSubHandlerPrivate *const d;
};

#endif // QHTTPENGINE_QSUBHANDLER_H
