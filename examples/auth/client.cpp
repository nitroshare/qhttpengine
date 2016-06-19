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

#include <iostream>

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include <QHttpEngine/QLocalFile>

int main(int argc, char * argv[])
{
    QCoreApplication a(argc, argv);

    // Attempt to open the local file and read from it
    QLocalFile file;
    if (!file.open()) {
        qCritical("Unable to open local file - is server running?");
        return 1;
    }

    // Parse the JSON to get the port and token
    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    if (!obj.contains("port") || !obj.contains("token")) {
        qCritical("Malformed JSON in local file.");
        return 1;
    }

    // Create a request to the server, using the provided port and passing the
    // auth token as a custom HTTP header
    QUrl url(QString("http://127.0.0.1:%1/").arg(obj.value("port").toInt()));
    QNetworkRequest request(url);
    request.setRawHeader("X-Auth-Token", obj.value("token").toString().toUtf8());

    // Send the request
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(request);

    // Check the response
    QObject::connect(reply, &QNetworkReply::finished, [&a, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug("Successfully authenticated to server.");
            a.exit();
        } else {
            qCritical("Error: %s", reply->errorString().toUtf8().constData());
            a.exit(1);
        }
    });

    return a.exec();
}
