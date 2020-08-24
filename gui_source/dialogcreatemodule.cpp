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
#include "dialogcreatemodule.h"
#include "ui_dialogcreatemodule.h"

DialogCreateModule::DialogCreateModule(QWidget *pParent) :
    QDialog(pParent),
    ui(new Ui::DialogCreateModule)
{
    ui->setupUi(this);

    mdata={};

    _currentDate();
    ui->pushButtonSave->setEnabled(false);
}

DialogCreateModule::~DialogCreateModule()
{
    delete ui;
}

void DialogCreateModule::on_pushButtonLoad_clicked()
{
//    QString sInitDirectory=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sInitDirectory;

    QString sFileName=QFileDialog::getOpenFileName(this,tr("Load project"),sInitDirectory,"*.pro.ini");

    if(sFileName!="")
    {
        QSettings settings(sFileName,QSettings::IniFormat);

        ui->lineEditName->setText(settings.value("Name","").toString());
        ui->lineEditVersion->setText(settings.value("Version","").toString());
        ui->dateEdit->setDate(QDate::fromString(settings.value("Date",QDate::currentDate().toString("yyyy-MM-dd")).toString(),"yyyy-MM-dd"));
        ui->lineEditAuthor->setText(settings.value("Author","").toString());
        ui->lineEditBugreport->setText(settings.value("Bugreport","").toString());
        ui->lineEditInfo->setText(settings.value("Info","").toString());
        ui->lineEditRoot->setText(settings.value("Root","").toString());
    }
}

void DialogCreateModule::on_pushButtonSave_clicked()
{
//    QString sFileName=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+QDir::separator()+ui->lineEditName->text()+".pro.ini";
    QString sFileName;

    sFileName=QFileDialog::getSaveFileName(this,tr("Save project"),sFileName,"*.pro.ini");

    if(sFileName!="")
    {
        QSettings settings(sFileName,QSettings::IniFormat);

        settings.setValue("Name",ui->lineEditName->text());
        settings.setValue("Version",ui->lineEditVersion->text());
        settings.setValue("Date",ui->dateEdit->date().toString("yyyy-MM-dd"));
        settings.setValue("Author",ui->lineEditAuthor->text());
        settings.setValue("Bugreport",ui->lineEditBugreport->text());
        settings.setValue("Info",ui->lineEditInfo->text());
        settings.setValue("Root",ui->lineEditRoot->text());
    }
}

void DialogCreateModule::on_pushButtonCreate_clicked()
{
    QString sFileName=Utils::createBundleName(&mdata)+".x64dbg.zip";
    sFileName=QFileDialog::getSaveFileName(this,tr("Save plugin bundle"),sFileName,"*.x64dbg.zip");

    if(sFileName!="")
    {
        mdata.sBundleFileName=sFileName;

        QString sErrorString;

        if(Utils::checkMData(&mdata,&sErrorString))
        {
            DialogCreateModuleProcess dcmp(this,&mdata,true);

            connect(&dcmp,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

            dcmp.exec();
        }
        else
        {
            emit errorMessage(sErrorString);
        }
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

    QString _sDirectoryName=sDirectoryName;

    if(sDirectoryName!="")
    {
        _sDirectoryName=XBinary::convertPathName(_sDirectoryName);

        QDir dir(_sDirectoryName);

        if(dir.exists())
        {
            mdata.sRoot=_sDirectoryName;

            QList<Utils::RECORD> listRecords=Utils::getRecords(_sDirectoryName);

            int nCount=listRecords.count();

            ui->tableWidgetRecords->setColumnCount(1);
            ui->tableWidgetRecords->setRowCount(nCount);

            for(int i=0;i<nCount;i++)
            {
                QTableWidgetItem *pItem=new QTableWidgetItem(listRecords.at(i).sPath);
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

void DialogCreateModule::on_pushButtonCurrentDate_clicked()
{
    _currentDate();
}

void DialogCreateModule::_currentDate()
{
    ui->dateEdit->setDate(QDate::currentDate());
}

void DialogCreateModule::on_dateEdit_dateChanged(const QDate &date)
{
    mdata.sDate=date.toString("yyyy-MM-dd");
}

void DialogCreateModule::on_lineEditInfo_textChanged(const QString &sInfo)
{
    mdata.sInfo=sInfo;
}

void DialogCreateModule::on_lineEditAuthor_textChanged(const QString &sAuthor)
{
    mdata.sAuthor=sAuthor;
}

void DialogCreateModule::on_lineEditBugreport_textChanged(const QString &sBugreport)
{
    mdata.sBugreport=sBugreport;
}
