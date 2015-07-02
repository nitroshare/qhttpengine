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

#include <QFile>
#include <QFileInfo>

#include "qfilesystemhandler.h"
#include "qfilesystemhandler_p.h"
#include "qiodevicecopier.h"

QFilesystemHandlerPrivate::QFilesystemHandlerPrivate(QFilesystemHandler *handler, const QString &root)
    : QObject(handler),
      q(handler),
      root(root)
{
}

bool QFilesystemHandlerPrivate::absolutePath(const QString &path, QString &absolutePath)
{
    // Clean the path and make it absolute
    absolutePath = QDir(root.absoluteFilePath(path)).canonicalPath();

    // Ensure that the absolute path is within the root
    return absolutePath.startsWith(root.canonicalPath());
}

QByteArray QFilesystemHandlerPrivate::mimeType(const QString &path)
{
    // TODO: use libmagic or the Windows registry when possible
    // TODO: determine what the equivalent is on OS X (libmagic?)

    QFileInfo info(path);
    QString extension = info.completeSuffix();

    if(extension == "css") { return "text/css"; }
    else if(extension == "js") { return "application/javascript"; }
    else if(extension == "jpg") { return "image/jpeg"; }
    else if(extension == "png") { return "image/png"; }
    else { return "application/octet-stream"; }
}

QFilesystemHandler::QFilesystemHandler(const QString &root, QObject *parent)
    : QHttpHandler(parent),
      d(new QFilesystemHandlerPrivate(this, root))
{
}

bool QFilesystemHandler::process(QHttpSocket *socket, const QString &path)
{
    // Attempt to retrieve the absolute path
    QString absolutePath;
    if(!d->absolutePath(path, absolutePath)) {
        return false;
    }

    // Attempt to open the file for reading
    QFile *file = new QFile(absolutePath);
    if(!file->open(QIODevice::ReadOnly)) {
        delete file;
        return false;
    }

    // Create a QIODeviceCopier to copy the file contents to the socket
    QIODeviceCopier *copier = new QIODeviceCopier(file, socket);
    connect(copier, SIGNAL(finished()), copier, SLOT(deleteLater()));
    connect(copier, SIGNAL(finished()), file, SLOT(deleteLater()));
    connect(copier, SIGNAL(finished()), socket, SLOT(deleteLater()));

    // Set the mimetype and contentlength
    socket->setHeader("Content-Type", d->mimeType(absolutePath));
    socket->setHeader("Content-Length", QByteArray::number(file->size()));
    socket->writeHeaders();

    // Start the copy and indicate success
    copier->start();
    return true;
}
