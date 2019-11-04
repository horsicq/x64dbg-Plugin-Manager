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
#include "dialogoptions.h"
#include "ui_dialogoptions.h"

DialogOptions::DialogOptions(QWidget *parent, XPLUGINMANAGER::OPTIONS *pOptions) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    this->pOptions=pOptions;

    ui->checkBoxStayOnTop->setChecked(pOptions->bStayOnTop);
}

DialogOptions::~DialogOptions()
{
    delete ui;
}

void DialogOptions::loadOptions(XPLUGINMANAGER::OPTIONS *pOptions)
{
    QSettings settings(QApplication::applicationDirPath()+QDir::separator()+"PluginManager.ini",QSettings::IniFormat);

    pOptions->bStayOnTop=settings.value("StayOnTop",false).toBool();
}

void DialogOptions::saveOptions(XPLUGINMANAGER::OPTIONS *pOptions)
{
    QSettings settings(QApplication::applicationDirPath()+QDir::separator()+"PluginManager.ini",QSettings::IniFormat);

    settings.setValue("StayOnTop",pOptions->bStayOnTop);
}

void DialogOptions::on_pushButtonOK_clicked()
{
    pOptions->bStayOnTop=ui->checkBoxStayOnTop->isChecked();

    this->close();
}

void DialogOptions::on_pushButtonCancel_clicked()
{
    this->close();
}
