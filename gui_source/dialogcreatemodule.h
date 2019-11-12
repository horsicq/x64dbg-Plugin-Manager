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
#ifndef DIALOGCREATEMODULE_H
#define DIALOGCREATEMODULE_H

#include <QDialog>
#include <QFileDialog>
#include <QStandardPaths>

namespace Ui {
class DialogCreateModule;
}

class DialogCreateModule : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCreateModule(QWidget *parent=nullptr);
    ~DialogCreateModule();

private slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonLoad_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonCreate_clicked();
    void on_toolButtonRoot_clicked();

    void on_lineEditRoot_textChanged(const QString &sDirectoryName);

private:
    Ui::DialogCreateModule *ui;
};

#endif // DIALOGCREATEMODULE_H
