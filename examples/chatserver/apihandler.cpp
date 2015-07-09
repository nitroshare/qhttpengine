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

#include "apihandler.h"

QVariantMap ApiHandler::postMessage(const QVariantMap &params)
{
    // Ensure that a valid message was supplied
    if(!params.contains("message")) {
        return QVariantMap();
    }

    // Add the new message to the list
    mMessages.append(params.value("message").toString());
    return QVariantMap();
}

QVariantMap ApiHandler::getMessages(const QVariantMap &params)
{
    // Ensure an index was supplied
    if(!params.contains("index")) {
        return QVariantMap();
    }

    int index = params.value("index").toInt();
    QVariantList messages;

    // Construct a list of all messages with an index higher than the one
    // that was provided as a parameter
    if(index >= -1 && index < mMessages.count()) {
        for(QStringList::const_iterator i = mMessages.constBegin() + index + 1;
                i != mMessages.constEnd(); ++i) {
            QVariantMap data;
            data.insert("index", i - mMessages.constBegin());
            data.insert("message", *i);
            messages.append(data);
        }
    }

    // Return the list of messages
    QVariantMap data;
    data.insert("messages", messages);
    return data;
}
