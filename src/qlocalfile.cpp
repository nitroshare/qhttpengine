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

#include <QCoreApplication>
#include <QDir>

#if defined(Q_OS_UNIX)
#  include <sys/stat.h>
#elif defined(Q_OS_WIN)
#  include <windows.h>
#endif

#include "qlocalfile.h"
#include "qlocalfile_p.h"

QLocalFilePrivate::QLocalFilePrivate(QLocalFile *localFile)
    : QObject(localFile),
      q(localFile)
{
    // Store the file in the user's home directory and set the filename to the
    // name of the application with a "." prepended
    q->setFileName(QDir::home().absoluteFilePath("." + QCoreApplication::applicationName()));
}

bool QLocalFilePrivate::setPermission()
{
#if defined(Q_OS_UNIX)
    return chmod(q->fileName().toUtf8().constData(), S_IRUSR | S_IWUSR) == 0;
#else
    // Unsupported platform, so setPermission() must fail
    return false;
#endif
}

bool QLocalFilePrivate::setHidden()
{
#if defined(Q_OS_UNIX)
    // On Unix, anything beginning with a "." is hidden
    return true;
#elif defined(Q_OS_WIN)
    return SetFileAttributesW((LPCWSTR)q->fileName().utf16(), FILE_ATTRIBUTE_HIDDEN) == 0;
#else
    // Unsupported platform, so setHidden() must fail
    return false;
#endif
}

QLocalFile::QLocalFile(QObject *parent)
    : QFile(parent),
      d(new QLocalFilePrivate(this))
{
}

bool QLocalFile::open()
{
    return QFile::open(QIODevice::WriteOnly) && d->setPermission() && d->setHidden();
}
