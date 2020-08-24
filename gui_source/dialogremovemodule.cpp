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
#include "dialogremovemodule.h"
#include "ui_dialogremovemodule.h"

DialogRemoveModule::DialogRemoveModule(QWidget *pParent, QString sDataPath, QString sRootPath, QString sModuleName) :
    QDialog(pParent),
    ui(new Ui::DialogRemoveModule)
{
    ui->setupUi(this);

    this->sDataPath=sDataPath;
    this->sRootPath=sRootPath;
    this->sModuleName=sModuleName;

    QString sFileName=Utils::getInstalledJsonFileName(sDataPath,sModuleName);

    Utils::MDATA mdata=Utils::getMDataFromJSONFile(sFileName);

    ui->widgetInfo->setData(&mdata);
}

DialogRemoveModule::~DialogRemoveModule()
{
    delete ui;
}

void DialogRemoveModule::on_pushButtonCancel_clicked()
{
    this->close();
}

void DialogRemoveModule::on_pushButtonOK_clicked()
{
    DialogRemoveModuleProcess drmp(this,sDataPath,sRootPath,QList<QString>()<<sModuleName);

    connect(&drmp,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

    drmp.exec();

    this->close();
}
