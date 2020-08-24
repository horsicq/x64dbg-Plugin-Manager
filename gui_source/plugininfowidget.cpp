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
#include "plugininfowidget.h"
#include "ui_plugininfowidget.h"

PluginInfoWidget::PluginInfoWidget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::PluginInfoWidget)
{
    ui->setupUi(this);
}

PluginInfoWidget::~PluginInfoWidget()
{
    delete ui;
}

void PluginInfoWidget::setData(Utils::MDATA *pMData)
{
    QString sText;

    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Name")).arg(pMData->sName);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Version")).arg(pMData->sVersion);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Date")).arg(pMData->sDate);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Author")).arg(pMData->sAuthor);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Bugreport")).arg(pMData->sBugreport);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Info")).arg(pMData->sInfo);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Size")).arg(pMData->nSize);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Compressed size")).arg(pMData->nCompressedSize);
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Is32")).arg((pMData->bIs32)?("true"):("false"));
    sText+=QString("<b>%1:</b> %2<br />").arg(tr("Is64")).arg((pMData->bIs64)?("true"):("false"));

    ui->textEditInfo->setHtml(sText);
}
