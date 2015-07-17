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

#ifndef QHTTPENGINE_QHTTPSOCKETPRIVATE_H
#define QHTTPENGINE_QHTTPSOCKETPRIVATE_H

#include <QObject>
#include <QTcpSocket>

#include "QHttpEngine/qhttpparser.h"
#include "QHttpEngine/qhttpsocket.h"

class QHttpSocketPrivate : public QObject
{
    Q_OBJECT

public:

    QHttpSocketPrivate(QHttpSocket *httpSocket, QTcpSocket *tcpSocket);

    QByteArray statusReason(int statusCode) const;

    QTcpSocket *socket;
    QByteArray readBuffer;

    enum {
        ReadHeaders,
        ReadData,
        ReadFinished
    } readState;

    QByteArray requestMethod;
    QByteArray requestPath;
    QHttpHeaderMap requestHeaders;
    qint64 requestDataRead;
    qint64 requestDataTotal;

    enum {
        WriteNone,
        WriteHeaders,
        WriteData,
        WriteFinished
    } writeState;

    int responseStatusCode;
    QByteArray responseStatusReason;
    QHttpHeaderMap responseHeaders;
    qint64 responseHeaderRemaining;

private Q_SLOTS:

    void onReadyRead();
    void onBytesWritten(qint64 bytes);

private:

    bool readHeaders();
    void readData();

    QHttpSocket *const q;
};

#endif // QHTTPENGINE_QHTTPSOCKETPRIVATE_H
