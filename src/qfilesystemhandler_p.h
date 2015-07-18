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

#ifndef QHTTPENGINE_QFILESYSTEMHANDLERPRIVATE_H
#define QHTTPENGINE_QFILESYSTEMHANDLERPRIVATE_H

#include <QDir>
#include <QMimeDatabase>
#include <QObject>

#include "QHttpEngine/qfilesystemhandler.h"
#include "QHttpEngine/qhttpsocket.h"

class QFilesystemHandlerPrivate : public QObject
{
    Q_OBJECT

public:

    QFilesystemHandlerPrivate(QFilesystemHandler *handler);

    bool absolutePath(const QString &path, QString &absolutePath);
    QByteArray mimeType(const QString &path);

    void processFile(QHttpSocket *socket, const QString &absolutePath);
    void processDirectory(QHttpSocket *socket, const QString &path, const QString &absolutePath);

    QDir documentRoot;
    QMimeDatabase database;
};

#endif // QHTTPENGINE_QFILESYSTEMHANDLERPRIVATE_H
