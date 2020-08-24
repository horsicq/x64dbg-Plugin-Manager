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
#ifndef GUIMAINWINDOW_H
#define GUIMAINWINDOW_H

#include <QMainWindow>
#include "../global.h"
#include "dialogcreatemodule.h"
#include "dialoginstallmodule.h"
#include "dialogremovemodule.h"
#include "dialogupdategitprocess.h"
#include "dialoginfomodule.h"
#include "dialogoptions.h"
#include "dialoggetfilefromserverprocess.h"
#include "dialogabout.h"
#include <QMessageBox>
#include <QCheckBox>
#include <QToolButton>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTableWidget>
#include <QDesktopServices>

QT_BEGIN_NAMESPACE
namespace Ui { class GuiMainWindow; }
QT_END_NAMESPACE

class GuiMainWindow : public QMainWindow
{
    Q_OBJECT

    enum CN
    {
        CN_NAME=0,
        CN_INFO,
        CN_32,
        CN_64,
        CN_VERSION,
        CN_DATE,
        CN_INSTALL,
        CN_REMOVE,
        CN_size
    };

public:
    GuiMainWindow(QWidget *pParent=nullptr);
    ~GuiMainWindow();

private:
    void adjustTable(QTableWidget *pTableWidget);
    void fillTable(QTableWidget *pTableWidget,QList<Utils::MDATA> *pMData,QMap<QString,Utils::STATUS> *pMapStatus);

private slots:
    void createPlugin();
    void aboutDialog();
    void openPlugin();
    void optionsDialog();
    void exitProgram();
    void errorMessage(QString sMessage);
    void getModules();
    void openPlugin(QString sFileName);
    void updateJsonList();
    void installButtonSlot();
    void removeButtonSlot();
    void installPlugin(QString sName);
    void installPlugins(QList<QString> *pListNames);
    void removePlugin(QString sName);
    void infoPlugin(Utils::MDATA *pMData);
    void on_tableWidgetServerList_customContextMenuRequested(const QPoint &pos);
    void on_tableWidgetInstalled_customContextMenuRequested(const QPoint &pos);
    void _infoPluginServerList();
    void _installPluginServerList();
    void _removePluginServerList();
    void _infoPluginInstalled();
    void _installPluginInstalled();
    void _removePluginInstalled();
    void updateServerList();
    void updateAllInstalledPlugins();
    void checkForUpdates();
    void on_pushButtonUpdateServerList_clicked();
    void on_pushButtonUpdateAllInstalledPlugins_clicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void on_actionCreate_triggered();
    void on_actionOptions_triggered();
    void on_actionUpdate_server_list_triggered();
    void on_actionUpdate_all_installed_plugins_triggered();
    void on_actionCheck_for_updates_triggered();

protected:
    void dragEnterEvent(QDragEnterEvent *pEvent) override;
    void dragMoveEvent(QDragMoveEvent *pEvent) override;
    void dropEvent(QDropEvent *pEvent) override;

private:
    Ui::GuiMainWindow *ui;
    XOptions xOptions;
    Utils::MODULES_DATA modulesData;
};
#endif // GUIMAINWINDOW_H
