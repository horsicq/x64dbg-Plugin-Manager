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
    // TODO
    QString sInitDirectory=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString sFileName=QFileDialog::getSaveFileName(this,tr("Save project"),sInitDirectory,"*.pro.ini");

    if(sFileName!="")
    {
        // TODO
    }
}

void DialogCreateModule::on_pushButtonCreate_clicked()
{
    // TODO
}

void DialogCreateModule::on_toolButtonRoot_clicked()
{
    QString sDirectoryName=QFileDialog::getExistingDirectory(this,tr("Select plugin root directory"),ui->lineEditRoot->text());

    if(sDirectoryName!="")
    {
        ui->lineEditRoot->setText(sDirectoryName);
        // TODO
    }
}
