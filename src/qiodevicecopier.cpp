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

#include <QTimer>

#include "qiodevicecopier.h"
#include "qiodevicecopier_p.h"

// Default value for the bufferSize property
const qint64 DefaultBufferSize = 65536;

QIODeviceCopierPrivate::QIODeviceCopierPrivate(QIODeviceCopier *copier, QIODevice *srcDevice, QIODevice *destDevice)
    : QObject(copier),
      q(copier),
      src(srcDevice),
      dest(destDevice),
      bufferSize(DefaultBufferSize)
{
}

void QIODeviceCopierPrivate::onReadyRead()
{
    dest->write(src->readAll());
}

void QIODeviceCopierPrivate::onReadChannelFinished()
{
    // Read any data that remains and signal the end of the operation
    onReadyRead();
    Q_EMIT q->finished();
}

void QIODeviceCopierPrivate::nextBlock()
{
    // Read an amount of data up to the size of the buffer
    dest->write(src->read(DefaultBufferSize));

    // Check if the end of the device has been reached - if so,
    // emit the finished signal and if not, continue to read
    // data at the next iteration of the event loop
    if(src->atEnd()) {
        Q_EMIT q->finished();
    } else {
        QTimer::singleShot(0, this, SLOT(nextBlock()));
    }
}

void QIODeviceCopierPrivate::onAboutToClose()
{
    Q_EMIT q->finished();
}

QIODeviceCopier::QIODeviceCopier(QIODevice *src, QIODevice *dest, QObject *parent)
    : QObject(parent),
      d(new QIODeviceCopierPrivate(this, src, dest))
{
}

qint64 QIODeviceCopier::bufferSize() const
{
    return d->bufferSize;
}

void QIODeviceCopier::setBufferSize(qint64 size)
{
    d->bufferSize = size;
}

void QIODeviceCopier::start()
{
    if(!d->src->isReadable()) {
        if(!d->src->open(QIODevice::ReadOnly)) {
            Q_EMIT error(tr("Unable to open source device for reading"));
            Q_EMIT finished();
            return;
        }
    }

    if(!d->dest->isWritable()) {
        if(!d->dest->open(QIODevice::WriteOnly)) {
            Q_EMIT error(tr("Unable to open destination device for writing"));
            Q_EMIT finished();
            return;
        }
    }

    // These signals cannot be connected in the constructor since they may
    // begin firing before the start() method is called

    // readyRead() and readChannelFinished() are only emitted for sequential
    // devices - for other types of devices, it is necessary to check atEnd()
    // in order to determine whether the end of the device has been reached
    connect(d->src, SIGNAL(readyRead()), d, SLOT(onReadyRead()));
    connect(d->src, SIGNAL(readChannelFinished()), d, SLOT(onReadChannelFinished()));
    connect(d->src, SIGNAL(aboutToClose()), d, SLOT(onAboutToClose()));

    // The first read from the device needs to be triggered
    QTimer::singleShot(0, d, d->src->isSequential() ? SLOT(onReadyRead()) : SLOT(nextBlock()));
}
