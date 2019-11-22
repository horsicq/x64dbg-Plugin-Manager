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
#include "dialogcreatemodule.h"
#include "ui_dialogcreatemodule.h"

DialogCreateModule::DialogCreateModule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateModule)
{
    ui->setupUi(this);

    mdata={};

    ui->pushButtonSave->setEnabled(false);
}

DialogCreateModule::~DialogCreateModule()
{
    delete ui;
}

void DialogCreateModule::on_pushButtonLoad_clicked()
{
    QString sInitDirectory=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sFileName=QFileDialog::getOpenFileName(this,tr("Load project"),sInitDirectory,"*.pro.ini");

    if(sFileName!="")
    {
        QSettings settings(sFileName,QSettings::IniFormat);

        ui->lineEditName->setText(settings.value("Name","").toString());
        // TODO
    }
}

void DialogCreateModule::on_pushButtonSave_clicked()
{
    QString sFileName=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+QDir::separator()+ui->lineEditName->text()+".pro.ini";
    sFileName=QFileDialog::getSaveFileName(this,tr("Save project"),sFileName,"*.pro.ini");

    if(sFileName!="")
    {
        QSettings settings(sFileName,QSettings::IniFormat);

        settings.setValue("Name",ui->lineEditName->text());
        // TODO
    }
}

void DialogCreateModule::on_pushButtonCreate_clicked()
{
    QString sErrorString;

    if(Utils::checkMData(&mdata,&sErrorString))
    {
        QString sFileName=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+QDir::separator()+Utils::createBundleName(&mdata)+".zip";
        sFileName=QFileDialog::getSaveFileName(this,tr("Save plugin bundle"),sFileName,"*.zip");

        if(sFileName!="")
        {
            // TODO
            QFileInfo fi(sFileName);
    //        QString sBundleName=fi.absoluteDir().path()+QDir::separator()+fi.baseName()+".zip";
    //        QString sJsonName=fi.absoluteDir().path()+QDir::separator()+fi.baseName()+".json";
            mdata.sBundleName=fi.baseName();
            mdata.sBundlePath=fi.absoluteDir().path();

            int nCount=mdata.listRecords.count();

            for(int i=0;i<nCount;i++)
            {
                QFile file;

                file.setFileName(mdata.listRecords.at(i).sFullPath);

                if(file.open(QIODevice::ReadOnly))
                {
                    // TODO
                    file.close();
                }
            }

            DialogCreateModuleProcess dcmp(this,&mdata);

            dcmp.exec();
        }
    }
    else
    {
        emit errorMessage(sErrorString);
    }
}

void DialogCreateModule::on_toolButtonRoot_clicked()
{
    QString sDirectoryName=QFileDialog::getExistingDirectory(this,tr("Select plugin root directory"),ui->lineEditRoot->text());

    if(sDirectoryName!="")
    {
        ui->lineEditRoot->setText(sDirectoryName);
    }
}

void DialogCreateModule::on_lineEditRoot_textChanged(const QString &sDirectoryName)
{
    mdata.sRoot="";

    if(sDirectoryName!="")
    {
        QDir dir(sDirectoryName);

        if(dir.exists())
        {
            mdata.sRoot=sDirectoryName;

            mdata.listRecords=Utils::getRecords(sDirectoryName);

            int nCount=mdata.listRecords.count();

            ui->tableWidgetRecords->setColumnCount(1);
            ui->tableWidgetRecords->setRowCount(nCount);

            for(int i=0;i<nCount;i++)
            {
                QTableWidgetItem *pItem=new QTableWidgetItem(mdata.listRecords.at(i).sPath);
                ui->tableWidgetRecords->setItem(i,0,pItem);
            }
        }
    }
}

void DialogCreateModule::on_pushButtonCancel_clicked()
{
    this->close();
}

void DialogCreateModule::on_lineEditName_textChanged(const QString &sName)
{
    QSignalBlocker(ui->lineEditName);

    QString _sName=sName;

    if(_sName.contains(" ")) _sName.remove(" ");
    // TODO more checks

    ui->lineEditName->setText(_sName);

    mdata.sName=_sName;

    ui->pushButtonSave->setEnabled(_sName!="");
}

void DialogCreateModule::on_lineEditVersion_textChanged(const QString &sVersion)
{
    // TODO Checks
    mdata.sVersion=sVersion;
}

void DialogCreateModule::on_lineEditDate_textChanged(const QString &sDate)
{
    // TODO Checks
    mdata.sDate=sDate;
}
