// Copyright (c) 2019 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "dialoggetfilefromserver.h"
#include "ui_dialoggetfilefromserver.h"

DialogGetFileFromServer::DialogGetFileFromServer(QWidget *parent,QString sLink, QString sFileName) :
    QDialog(parent),
    ui(new Ui::DialogGetFileFromServer)
{
    ui->setupUi(this);

    ui->setupUi(this);

    pGetFileFromServerProcess=new GetFileFromServerProcess;
    pThread=new QThread;

    pGetFileFromServerProcess->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pGetFileFromServerProcess, SLOT(process()));
    connect(pGetFileFromServerProcess, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pGetFileFromServerProcess,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    bIsRun=false;

    pGetFileFromServerProcess->setData(sLink,sFileName);

    bIsRun=true;

    pThread->start();
}

DialogGetFileFromServer::~DialogGetFileFromServer()
{
    delete ui;
}

void DialogGetFileFromServer::on_pushButtonCancel_clicked()
{
    if(bIsRun)
    {
        pGetFileFromServerProcess->stop();
        bIsRun=false;
    }
}

void DialogGetFileFromServer::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)
    // TODO
    bIsRun=false;
    this->close();
}
