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

#ifndef QHTTPENGINE_QLOCALFILE_H
#define QHTTPENGINE_QLOCALFILE_H

#include <QFile>

#include "qhttpengine.h"

class QHTTPENGINE_EXPORT QLocalFilePrivate;

/**
 * @brief Local file accessible
 * @headerfile qlocalauth.h QLocalAuth
 *
 * QLocalFile uses platform-specific functions to create a file containing
 * information that will be accessible only to the local user. This is
 * typically used for storing authentication tokens.
 */
class QHTTPENGINE_EXPORT QLocalFile : public QFile
{
    Q_OBJECT

public:

    /**
     * @brief Create a new local file
     */
    explicit QLocalFile(QObject *parent = 0);

    /**
     * @brief Attempt to open the file
     *
     * The file must be opened before data can be written.
     */
    bool open();

private:

    QLocalFilePrivate *const d;
};

#endif // QHTTPENGINE_QLOCALFILE_H
