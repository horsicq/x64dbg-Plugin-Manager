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
}

DialogCreateModule::~DialogCreateModule()
{
    delete ui;
}

void DialogCreateModule::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogCreateModule::on_pushButtonLoad_clicked()
{
    QString sInitDirectory=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sFileName=QFileDialog::getOpenFileName(this,tr("Load project"),sInitDirectory,"*.pro.ini");

    if(sFileName!="")
    {
        // TODO
    }
}

void DialogCreateModule::on_pushButtonSave_clicked()
{
    QString sInitDirectory=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sFileName=QFileDialog::getSaveFileName(this,tr("Save project"),sInitDirectory,"*.pro.ini");

    if(sFileName!="")
    {
        // TODO
    }
}

void DialogCreateModule::on_pushButtonCreate_clicked()
{
    QString sInitDirectory=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sFileName=QFileDialog::getSaveFileName(this,tr("Save plugin bundle"),sInitDirectory,"*.zip");

    if(sFileName!="")
    {
        // TODO
        QFileInfo fi(sFileName);

        QString sBundleName=fi.absoluteDir().path()+QDir::separator()+fi.baseName()+".zip";
        QString sJsonName=fi.absoluteDir().path()+QDir::separator()+fi.baseName()+".json";

        int z=0;
        z++;
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
    if(sDirectoryName!="")
    {
        QDir dir(sDirectoryName);

        if(dir.exists())
        {
            listRecords=Utils::getRecords(sDirectoryName);

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
