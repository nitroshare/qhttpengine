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

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QHostAddress>
#include <QRegExp>
#include <QStringList>

#include <QHttpEngine/QFilesystemHandler>
#include <QHttpEngine/QHttpHandler>
#include <QHttpEngine/QHttpServer>

#include "apihandler.h"

int main(int argc, char * argv[])
{
    QCoreApplication a(argc, argv);

    // Build the command-line options
    QCommandLineParser parser;
    QCommandLineOption addressOption(
        QStringList() << "a" << "address",
        "address to bind to",
        "address",
        "127.0.0.1"
    );
    parser.addOption(addressOption);
    QCommandLineOption portOption(
        QStringList() << "p" << "port",
        "port to listen on",
        "port",
        "8000"
    );
    parser.addOption(portOption);
    parser.addHelpOption();

    // Parse the options that were provided
    parser.process(a);

    // Obtain the values
    QHostAddress address = QHostAddress(parser.value(addressOption));
    quint16 port = parser.value(portOption).toInt();

    // Build the hierarchy of handlers
    QFilesystemHandler handler(":/static");
    handler.addRedirect(QRegExp("^$"), "/index.html");

    ApiHandler apiHandler;
    handler.addSubHandler(QRegExp("api/"), &apiHandler);

    QHttpServer server(&handler);

    // Attempt to listen on the specified port
    if(!server.listen(address, port)) {
        qCritical("Unable to listen on the specified port.");
        return 1;
    }

    return a.exec();
}
