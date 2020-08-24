// Copyright (c) 2019-2020 hors<horsicq@gmail.com>
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
#include "dialoggetfilefromserverprocess.h"
#include "ui_dialoggetfilefromserverprocess.h"

DialogGetFileFromServerProcess::DialogGetFileFromServerProcess(QWidget *pParent, QList<Utils::WEB_RECORD> listWebRecords) :
    QDialog(pParent),
    ui(new Ui::DialogGetFileFromServerProcess)
{
    ui->setupUi(this);

    pGetFileFromServerProcess=new GetFileFromServerProcess;
    pThread=new QThread;

    pGetFileFromServerProcess->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pGetFileFromServerProcess, SLOT(process()));
    connect(pGetFileFromServerProcess, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pGetFileFromServerProcess,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    bIsRun=false;

    pGetFileFromServerProcess->setData(listWebRecords);

    bIsRun=true;

    pThread->start();

    pTimer=new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    ui->progressBarModule->setMaximum(100);
    ui->progressBarModule->setValue(0);

    ui->progressBarBytes->setMaximum(100);
    ui->progressBarBytes->setValue(0);

    pTimer->start(100); // 0.1 sec
}

DialogGetFileFromServerProcess::~DialogGetFileFromServerProcess()
{
    if(bIsRun)
    {
        pGetFileFromServerProcess->stop();
    }

    pTimer->stop();

    pThread->quit();
    pThread->wait();

    delete ui;

    delete pThread;
    delete pGetFileFromServerProcess;
}

void DialogGetFileFromServerProcess::on_pushButtonCancel_clicked()
{
    if(bIsRun)
    {
        pGetFileFromServerProcess->stop();
        pTimer->stop();
        bIsRun=false;
    }
}

void DialogGetFileFromServerProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)
    // TODO
    bIsRun=false;
    this->close();
}

void DialogGetFileFromServerProcess::timerSlot()
{
    Utils::STATS stats=pGetFileFromServerProcess->getCurrentStats();

    ui->labelInfoModule->setText(stats.sModule);

    if(stats.nTotalModule)
    {
        ui->progressBarModule->setValue((int)((stats.nCurrentModule*100)/stats.nTotalModule));
    }

    if(stats.nTotalBytes)
    {
        ui->progressBarBytes->setValue((int)((stats.nCurrentBytes*100)/stats.nTotalBytes));
    }
}
