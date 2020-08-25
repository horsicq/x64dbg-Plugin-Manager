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
#ifndef DIALOGINSTALLMODULE_H
#define DIALOGINSTALLMODULE_H

#include <QDialog>
#include "dialoginstallmoduleprocess.h"
#include "dialogconvertprocess.h"
#include "dialogcreatemoduleprocess.h"
#include "dialoggetfilefromserverprocess.h"
#include "../utils.h"

namespace Ui {
class DialogInstallModule;
}

class DialogInstallModule : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInstallModule(QWidget *pParent, QString sDataPath, QString sRootPath);
    ~DialogInstallModule();
    void setFileName(QString sModuleFileName);
    bool setMData(Utils::MDATA *pMData);

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();

signals:
    void errorMessage(QString sMessage);

private:
    Ui::DialogInstallModule *ui;
    QWidget *pParent;
    Utils::MDATA _mdata;
    QString sDataPath;
    QString sRootPath;
    QString sModuleFileName;
};

#endif // DIALOGINSTALLMODULE_H
