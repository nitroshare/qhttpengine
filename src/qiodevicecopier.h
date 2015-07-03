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

#ifndef QHTTPENGINE_QIODEVICECOPIER_H
#define QHTTPENGINE_QIODEVICECOPIER_H

#include <QIODevice>
#include <QObject>

#include "qhttpengine.h"

class QHTTPENGINE_EXPORT QIODeviceCopierPrivate;

/**
 * @brief Device copier
 * @headerfile qiodevicecopier.h QIODeviceCopier
 *
 * QIODeviceCopier provides a set of methods for reading data from a QIODevice
 * and writing it to another. The class operates asynchronously and therefore
 * can be used from the main thread.
 */
class QHTTPENGINE_EXPORT QIODeviceCopier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 bufferSize READ bufferSize WRITE setBufferSize)

public:

    /**
     * @brief Create a new device copier from the specified source and destination devices
     */
    QIODeviceCopier(QIODevice *src, QIODevice *dest, QObject *parent = 0);

    /**
     * @brief Retrieve the current buffer size
     */
    qint64 bufferSize() const;

    /**
     * @brief Set the size of the buffer
     */
    void setBufferSize(qint64 size);

Q_SIGNALS:

    /**
     * @brief Indicate that an error occurred
     */
    void error(const QString &message);

    /**
     * @brief Indicate that the copy operation finished
     *
     * For sequential devices, this will occur when readChannelFinished() is
     * emitted. For other files, this signal relies on QIODevice::atEnd() and
     * QIODevice::aboutToClose().
     *
     * This signal will also be emitted immediately after the error() signal.
     */
    void finished();

public Q_SLOTS:

    /**
     * @brief Begin the copy operation
     *
     * The source device will be opened for reading and the destination device
     * opened for writing if applicable. If opening either file fails for some
     * reason, the error() signal will be emitted.
     *
     * This method should never be invoked more than once.
     */
    void start();

private:

    QIODeviceCopierPrivate *const d;
    friend class QIODeviceCopierPrivate;
};

#endif // QHTTPENGINE_QIODEVICECOPIER_H
