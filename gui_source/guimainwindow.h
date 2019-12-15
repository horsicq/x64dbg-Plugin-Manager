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
#ifndef GUIMAINWINDOW_H
#define GUIMAINWINDOW_H

#include <QMainWindow>
#include "../global.h"
#include "dialogcreatemodule.h"
#include "dialoginstallmodule.h"
#include "dialogoptions.h"
#include "dialogreload.h"
#include "dialogabout.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class GuiMainWindow; }
QT_END_NAMESPACE

class GuiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GuiMainWindow(QWidget *parent=nullptr);
    ~GuiMainWindow();

private slots:
    void on_actionCreate_triggered();
    void on_pushButtonUpdateAll_clicked();
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void on_actionOptions_triggered();
    void on_pushButtonReload_clicked();
    void on_actionExit_triggered();
    void errorMessage(QString sMessage);
    void getModules();
    void openPlugin(QString sFileName);

private:
    Ui::GuiMainWindow *ui;
    XPLUGINMANAGER::OPTIONS options;
};
#endif // GUIMAINWINDOW_H
