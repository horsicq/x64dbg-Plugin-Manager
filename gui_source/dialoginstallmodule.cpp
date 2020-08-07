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
#include "dialoginstallmodule.h"
#include "ui_dialoginstallmodule.h"

DialogInstallModule::DialogInstallModule(QWidget *parent, QString sDataPath, QString sRootPath) :
    QDialog(parent),
    ui(new Ui::DialogInstallModule)
{
    ui->setupUi(this);

    this->sDataPath=sDataPath;
    this->sRootPath=sRootPath;
}

DialogInstallModule::~DialogInstallModule()
{
    delete ui;
}

void DialogInstallModule::setFileName(QString sModuleFileName)
{
    this->sModuleFileName=sModuleFileName;

    _mdata=Utils::getMDataFromZip(sModuleFileName,XBinary::convertPathName(sRootPath));

    ui->widgetInfo->setData(&_mdata);

    int nCount=_mdata.listInstallRecords.count();

    ui->tableWidgetRecords->setColumnCount(1);
    ui->tableWidgetRecords->setRowCount(nCount);

    for(int i=0;i<nCount;i++)
    {
        QTableWidgetItem *pItem=new QTableWidgetItem(_mdata.listInstallRecords.at(i).sPath);
        ui->tableWidgetRecords->setItem(i,0,pItem);
    }
}

void DialogInstallModule::setMData(Utils::MDATA *pMData)
{
    QString sFileName=Utils::getModuleFileName(sDataPath,pMData->sName);

    if(!XBinary::isFileHashValid(XBinary::HASH_SHA1,sFileName,pMData->sSHA1))
    {
        Utils::WEB_RECORD record={};

        record.sFileName=sFileName;
        record.sLink=pMData->sSrc;

        DialogGetFileFromServerProcess dialogGetFileFromServer(this,QList<Utils::WEB_RECORD>()<<record);

        connect(&dialogGetFileFromServer,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

        dialogGetFileFromServer.exec();
    }

    if(XBinary::isFileHashValid(XBinary::HASH_SHA1,sFileName,pMData->sSHA1))
    {
        setFileName(sFileName);
    }
    else
    {
        emit errorMessage(QString("%1: %2").arg(tr("Invalid SHA1")).arg(sFileName));
    }
}

void DialogInstallModule::on_pushButtonCancel_clicked()
{
    this->close();
}

void DialogInstallModule::on_pushButtonOK_clicked()
{
    DialogInstallModuleProcess dimp(this,sDataPath,sRootPath,QList<QString>()<<sModuleFileName);

    connect(&dimp,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    dimp.exec();

    this->close();
}
