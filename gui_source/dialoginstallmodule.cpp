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

DialogInstallModule::DialogInstallModule(QWidget *pParent, QString sDataPath, QString sRootPath) :
    QDialog(pParent),
    ui(new Ui::DialogInstallModule)
{
    ui->setupUi(this);

    this->pParent=pParent;
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

bool DialogInstallModule::setMData(Utils::MDATA *pMData)
{
    bool bResult=false;

    QString sSHA1=pMData->sSHA1;

    QString sModuleFileName=Utils::getModuleFileName(sDataPath,pMData->sName);

    bool bHash=XBinary::isFileHashValid(XBinary::HASH_SHA1,sModuleFileName,sSHA1);

    if(!bHash)
    {
        if(pMData->sGithub!="")
        {
            QString sConvertPath=Utils::getConvertPath(sDataPath,pMData->sName);
            QString sDownloadModulePath=Utils::getConvertModulePath(sDataPath,pMData->sName);

            XBinary::createDirectory(sConvertPath);
            XBinary::createDirectory(sDownloadModulePath);

            QList<Utils::WEB_RECORD> listWebRecords;

            int nCount=pMData->listDownloads.count();

            for(int i=0;i<nCount;i++)
            {
                Utils::WEB_RECORD record={};

                QString sLink=pMData->listDownloads.at(i);

                record.sFileName=sConvertPath+QDir::separator()+sLink.section("/",-1,-1);
                record.sFileName=record.sFileName.remove("?raw=true");
                record.sLink=sLink;

                listWebRecords.append(record);
            }

            DialogGetFileFromServerProcess dialogGetFileFromServer(pParent,listWebRecords);

            connect(&dialogGetFileFromServer,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

            dialogGetFileFromServer.exec();

            DialogConvertProcess dialogConvertProcess(pParent,pMData,sDataPath);

            connect(&dialogConvertProcess,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

            dialogConvertProcess.exec();

            Utils::MDATA mdata=*pMData;

            mdata.sBundleFileName=Utils::getModuleFileName(sDataPath,mdata.sName);
            mdata.sRoot=sDownloadModulePath;

            QString sErrorString;

            if(Utils::checkMData(&mdata,&sErrorString))
            {
                DialogCreateModuleProcess dcmp(pParent,&mdata,false);

                connect(&dcmp,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

                dcmp.exec();
            }
            else
            {
                emit errorMessage(sErrorString);
            }

        #ifndef QT_DEBUG
            XBinary::removeDirectory(sConvertPath);
        #endif

            Utils::updateJsonFile(Utils::getServerListFileName(sDataPath),QList<Utils::MDATA>() << mdata);

            sSHA1=mdata.sSHA1;
        }
        else
        {
            Utils::WEB_RECORD record={};

            record.sFileName=sModuleFileName;
            record.sLink=pMData->sSrc;

            DialogGetFileFromServerProcess dialogGetFileFromServer(pParent,QList<Utils::WEB_RECORD>()<<record);

            connect(&dialogGetFileFromServer,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

            dialogGetFileFromServer.exec();
        }

        bHash=XBinary::isFileHashValid(XBinary::HASH_SHA1,sModuleFileName,sSHA1);
    }

    if(bHash)
    {
        setFileName(sModuleFileName);

        bResult=true;
    }
    else
    {
        emit errorMessage(QString("%1: %2").arg(tr("Invalid SHA1")).arg(sModuleFileName));
    }

    return bResult;
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
