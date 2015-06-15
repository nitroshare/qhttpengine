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

#ifndef QHTTPENGINE_QFILESYSTEMHANDLER_H
#define QHTTPENGINE_QFILESYSTEMHANDLER_H

#include "config.h"
#include "qhttphandler.h"

class QHTTPENGINE_EXPORT QFilesystemHandlerPrivate;

/**
 * @brief Handler for filesystem requests
 */
class QHTTPENGINE_EXPORT QFilesystemHandler : public QHttpHandler
{
    Q_OBJECT

public:

    /**
     * @brief Create a new filesystem handler
     *
     * The root path provided is used to resolve each of the requests when
     * they are received.
     */
    explicit QFilesystemHandler(const QString &root, QObject *parent = 0);

    /**
     * @brief Reimplementation of QHttpHandler::process()
     */
    virtual bool process(QHttpSocket *socket, const QString &path);

private:

    QFilesystemHandlerPrivate *const d;
};

#endif // QHTTPENGINE_QFILESYSTEMHANDLER_H
