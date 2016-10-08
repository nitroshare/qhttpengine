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

#ifndef QHTTPENGINE_QFILESYSTEMHANDLER_H
#define QHTTPENGINE_QFILESYSTEMHANDLER_H

#include <QHttpEngine/QHttpHandler>

#include "qhttpengine_global.h"

class QHTTPENGINE_EXPORT QFilesystemHandlerPrivate;

/**
 * @brief Handler for filesystem requests
 * @headerfile qfilesystemhandler.h QHttpEngine/QFilesystemHandler
 *
 * This handler responds to requests for resources on a local filesystem. The
 * constructor is provided with a path to the root directory, which will be
 * used to resolve all paths. The following example creates a handler that
 * serves files from the /var/www directory:
 *
 * @code
 * QFilesystemHandler handler("/var/www");
 * @endcode
 *
 * Requests for resources outside the root will be ignored. The document root
 * can be modified after initialization. It is possible to use a resource
 * directory for the document root.
 */
class QHTTPENGINE_EXPORT QFilesystemHandler : public QHttpHandler
{
    Q_OBJECT

public:

    /**
     * @brief Create a new filesystem handler
     */
    explicit QFilesystemHandler(QObject *parent = 0);

    /**
     * @brief Create a new filesystem handler from the specified directory
     */
    QFilesystemHandler(const QString &documentRoot, QObject *parent = 0);

    /**
     * @brief Set the document root
     *
     * The root path provided is used to resolve each of the requests when
     * they are received.
     */
    void setDocumentRoot(const QString &documentRoot);

protected:

    /**
     * @brief Reimplementation of QHttpHandler::process()
     */
    virtual void process(QHttpSocket *socket, const QString &path);

private:

    QFilesystemHandlerPrivate *const d;
    friend class QFilesystemHandlerPrivate;
};

#endif // QHTTPENGINE_QFILESYSTEMHANDLER_H
