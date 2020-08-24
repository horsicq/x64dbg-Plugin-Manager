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
#include "dialogremovemoduleprocess.h"
#include "ui_dialogremovemoduleprocess.h"

DialogRemoveModuleProcess::DialogRemoveModuleProcess(QWidget *pParent, QString sDataPath, QString sRootPath, QList<QString> listModuleNames) :
    QDialog(pParent),
    ui(new Ui::DialogRemoveModuleProcess)
{
    ui->setupUi(this);

    this->sDataPath=sDataPath;
    this->sRootPath=sRootPath;
    this->listModuleNames=listModuleNames;

    pRemoveModuleProcess=new RemoveModuleProcess;
    pThread=new QThread;

    pRemoveModuleProcess->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pRemoveModuleProcess, SLOT(process()));
    connect(pRemoveModuleProcess, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pRemoveModuleProcess,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    pTimer=new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    pRemoveModuleProcess->setData(sDataPath,sRootPath,listModuleNames);

    bIsRun=true;

    ui->progressBarFile->setMaximum(100);
    ui->progressBarFile->setValue(0);

    ui->progressBarModule->setMaximum(100);
    ui->progressBarModule->setValue(0);

    pThread->start();
    pTimer->start(1000); // 1 sec
}

DialogRemoveModuleProcess::~DialogRemoveModuleProcess()
{
    if(bIsRun)
    {
        pRemoveModuleProcess->stop();
    }

    pTimer->stop();

    pThread->quit();
    pThread->wait();

    delete ui;

    delete pThread;
    delete pRemoveModuleProcess;
}

void DialogRemoveModuleProcess::on_pushButtonCancel_clicked()
{
    if(bIsRun)
    {
        pRemoveModuleProcess->stop();
        pTimer->stop();
        bIsRun=false;
    }
}

void DialogRemoveModuleProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)
    // TODO
    bIsRun=false;
    this->close();
}

void DialogRemoveModuleProcess::timerSlot()
{
    Utils::STATS stats=pRemoveModuleProcess->getCurrentStats();

    ui->labelInfoFile->setText(stats.sFile);

    if(stats.nTotalFile)
    {
        ui->progressBarFile->setValue((int)((stats.nCurrentFile*100)/stats.nTotalFile));
    }

    ui->labelInfoModule->setText(stats.sModule);

    if(stats.nTotalModule)
    {
        ui->progressBarModule->setValue((int)((stats.nCurrentModule*100)/stats.nTotalModule));
    }
}
