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
#ifndef DIALOGCREATEMODULE_H
#define DIALOGCREATEMODULE_H

#include <QDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>
#include <QDate>
#include "dialogcreatemoduleprocess.h"
#include "../utils.h"

namespace Ui {
class DialogCreateModule;
}

class DialogCreateModule : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCreateModule(QWidget *pParent);
    ~DialogCreateModule();

private slots:
    void on_pushButtonLoad_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonCreate_clicked();
    void on_toolButtonRoot_clicked();
    void on_lineEditRoot_textChanged(const QString &sDirectoryName);
    void on_pushButtonCancel_clicked();
    void on_lineEditName_textChanged(const QString &sName);
    void on_lineEditVersion_textChanged(const QString &sVersion);
    void on_pushButtonCurrentDate_clicked();
    void _currentDate();
    void on_dateEdit_dateChanged(const QDate &date);
    void on_lineEditInfo_textChanged(const QString &sInfo);
    void on_lineEditAuthor_textChanged(const QString &sAuthor);
    void on_lineEditBugreport_textChanged(const QString &sBugreport);

signals:
    void errorMessage(QString sMessage);

private:
    Ui::DialogCreateModule *ui;
    Utils::MDATA mdata;
};

#endif // DIALOGCREATEMODULE_H
