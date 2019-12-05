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
#include "dialoginstallmoduleprocess.h"
#include "ui_dialoginstallmoduleprocess.h"

DialogInstallModuleProcess::DialogInstallModuleProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInstallModuleProcess)
{
    ui->setupUi(this);

    this->pMData=pMData;

    pInstallModuleProcess=new InstallModuleProcess;
    pThread=new QThread;

    pInstallModuleProcess->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pInstallModuleProcess, SLOT(process()));
    connect(pInstallModuleProcess, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pInstallModuleProcess,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    bIsRun=false;

    pTimer=new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    pInstallModuleProcess->setData(pMData);

    bIsRun=true;

    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

    pThread->start();
    pTimer->start(1000); // 1 sec
}

DialogInstallModuleProcess::~DialogInstallModuleProcess()
{
    delete ui;
}

void DialogInstallModuleProcess::on_pushButtonCancel_clicked()
{
    if(bIsRun)
    {
        pInstallModuleProcess->stop();
        pTimer->stop();
        bIsRun=false;
    }
}

void DialogInstallModuleProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)
    // TODO
    bIsRun=false;
    this->close();
}

void DialogInstallModuleProcess::timerSlot()
{
    Utils::STATS stats=pInstallModuleProcess->getCurrentStats();

    ui->labelInfo->setText(stats.sStatus);

    if(stats.nTotal)
    {
        ui->progressBar->setValue((int)((stats.nCurrent*100)/stats.nTotal));
    }
}
