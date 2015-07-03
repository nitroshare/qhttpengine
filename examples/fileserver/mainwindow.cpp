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

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"

MainWindow::MainWindow()
    : mServer(&mHandler)
{
    setupUi(this);

    // Obtain the user's current home directory
    documentRoot->setText(QDir::homePath());
}

void MainWindow::onBrowseClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Browse"), documentRoot->text());
    if(!dir.isNull()) {
        documentRoot->setText(dir);
    }
}

void MainWindow::onStartServerClicked()
{
    mHandler.setDocumentRoot(documentRoot->text());

    if(mServer.listen(QHostAddress::Any, port->value())) {
        toggleWidgets(true);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Unable to listen on port."));
    }
}

void MainWindow::onStopServerClicked()
{
    mServer.close();
    toggleWidgets(false);
}

void MainWindow::toggleWidgets(bool running)
{
    documentRoot->setEnabled(!running);
    browse->setEnabled(!running);
    portLabel->setEnabled(!running);
    port->setEnabled(!running);
    startServer->setEnabled(!running);
    stopServer->setEnabled(running);
}
