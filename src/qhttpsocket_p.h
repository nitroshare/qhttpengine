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
 **/

#ifndef QHTTPENGINE_QHTTPSOCKETPRIVATE_H
#define QHTTPENGINE_QHTTPSOCKETPRIVATE_H

#include <QObject>
#include <QMap>
#include <QTcpSocket>

#include "qhttpsocket.h"

class QHttpSocketPrivate : public QObject
{
    Q_OBJECT

public:

    QHttpSocketPrivate(QHttpSocket *httpSocket, QTcpSocket *baseSocket);

    void writeResponseHeaders();

    QTcpSocket *socket;
    QByteArray buffer;

    QHttpSocket::Error error;

    QString requestMethod;
    QString requestUri;
    QMap<QString, QString> requestHeaders;
    bool requestHeadersRead;

    QString responseStatusCode;
    QMap<QString, QString> responseHeaders;
    qint64 responseHeaderLength;
    bool responseHeadersWritten;

private Q_SLOTS:

    void onReadyRead();
    void onBytesWritten(qint64 bytes);
    void onError(QAbstractSocket::SocketError socketError);

private:

    void abortWithError(QHttpSocket::Error socketError);

    void parseRequestHeaders(const QString &headers);
    void parseRequestLine(const QString &line);
    void parseRequestHeader(const QString &header);

    QHttpSocket *const q;
};

#endif // QHTTPENGINE_QHTTPSOCKETPRIVATE_H
