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
#include "guimainwindow.h"
#include "ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *pParent)
    : QMainWindow(pParent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString("%1 v%2").arg(X_APPLICATIONNAME).arg(X_APPLICATIONVERSION));

    setAcceptDrops(true);

    xOptions.setName(X_OPTIONSFILE);

    QList<XOptions::ID> listIDs;

    listIDs.append(XOptions::ID_STAYONTOP);
    listIDs.append(XOptions::ID_DATAPATH);
    listIDs.append(XOptions::ID_ROOTPATH);
    listIDs.append(XOptions::ID_JSON);

    xOptions.setValueIDs(listIDs);

    QMap<XOptions::ID, QVariant> mapDefaultValues;

    mapDefaultValues.insert(XOptions::ID_JSON,X_JSON_DEFAULT);

    xOptions.setDefaultValues(mapDefaultValues);

    xOptions.load();

    xOptions.adjustStayOnTop(this);

    if(!XBinary::isDirectoryExists(XBinary::convertPathName(xOptions.getRootPath())))
    {
        xOptions.clearValue(XOptions::ID_ROOTPATH);
    }

    if(xOptions.getRootPath()=="")
    {
        QString sDirectoryName;

        if(QMessageBox::information(this,
                                    tr("Information"),
                                    tr("Please select a root directory for x64dbg"),
                                    QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes)==QMessageBox::Yes)
        {
            sDirectoryName=QFileDialog::getExistingDirectory(this,tr("Select root directory"));
        }

        if(sDirectoryName!="")
        {
            xOptions.setValue(XOptions::ID_ROOTPATH,sDirectoryName);
        }
    }

    if(xOptions.getRootPath()=="")
    {
        exit(1);
    }

    XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath()));
    XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath())+QDir::separator()+"installed");
    XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath())+QDir::separator()+"modules");

    setAcceptDrops(true);

    if(QCoreApplication::arguments().count()>1)
    {
        QString sFileName=QCoreApplication::arguments().at(1);

        openPlugin(sFileName);
    }
    else
    {
        if(!XBinary::isFileExists(Utils::getServerListFileName(xOptions.getDataPath())))
        {
            updateJsonList();
        }
    }

    adjustTable(ui->tableWidgetServerList);
    adjustTable(ui->tableWidgetInstalled);

    getModules();
}

GuiMainWindow::~GuiMainWindow()
{
    xOptions.save();

    delete ui;
}

void GuiMainWindow::adjustTable(QTableWidget *pTableWidget)
{
    pTableWidget->setColumnCount(CN_size);
    pTableWidget->setRowCount(0);

    pTableWidget->setHorizontalHeaderItem(CN_NAME,          new QTableWidgetItem(tr("Name")));
    pTableWidget->setHorizontalHeaderItem(CN_INFO,          new QTableWidgetItem(tr("Information")));
    pTableWidget->setHorizontalHeaderItem(CN_32,            new QTableWidgetItem(tr("32")));
    pTableWidget->setHorizontalHeaderItem(CN_64,            new QTableWidgetItem(tr("64")));
    pTableWidget->setHorizontalHeaderItem(CN_VERSION,       new QTableWidgetItem(tr("Version")));
    pTableWidget->setHorizontalHeaderItem(CN_DATE,          new QTableWidgetItem(tr("Date")));
    pTableWidget->setHorizontalHeaderItem(CN_INSTALL,       new QTableWidgetItem(""));
    pTableWidget->setHorizontalHeaderItem(CN_REMOVE,        new QTableWidgetItem(""));

    pTableWidget->setColumnWidth(CN_NAME,                   100);
    pTableWidget->setColumnWidth(CN_INFO,                   300);
    pTableWidget->setColumnWidth(CN_32,                     10);
    pTableWidget->setColumnWidth(CN_64,                     10);
    pTableWidget->setColumnWidth(CN_VERSION,                80);
    pTableWidget->setColumnWidth(CN_DATE,                   80);
    pTableWidget->setColumnWidth(CN_INSTALL,                60);
    pTableWidget->setColumnWidth(CN_REMOVE,                 60);

    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_NAME,         QHeaderView::ResizeToContents);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_INFO,         QHeaderView::Stretch);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_32,           QHeaderView::ResizeToContents);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_64,           QHeaderView::ResizeToContents);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_VERSION,      QHeaderView::ResizeToContents);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_DATE,         QHeaderView::ResizeToContents);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_INSTALL,      QHeaderView::Interactive);
    pTableWidget->horizontalHeader()->setSectionResizeMode(CN_REMOVE,       QHeaderView::Interactive);
}

void GuiMainWindow::fillTable(QTableWidget *pTableWidget, QList<Utils::MDATA> *pMData, QMap<QString, Utils::STATUS> *pMapStatus)
{
    int nCount=pMData->count();

    pTableWidget->setSortingEnabled(false);

    pTableWidget->setRowCount(0);
    pTableWidget->setRowCount(nCount);

    QColor colDisabled=QWidget::palette().color(QPalette::Window);

    for(int i=0;i<nCount;i++)
    {
        Utils::STATUS status=pMapStatus->value(pMData->at(i).sName);

        QTableWidgetItem *pItemName=new QTableWidgetItem;
        QTableWidgetItem *pItemInfo=new QTableWidgetItem;
        QCheckBox *pCheckBoxIs32=new QCheckBox(this);
        QCheckBox *pCheckBoxIs64=new QCheckBox(this);
        QTableWidgetItem *pItemVersion=new QTableWidgetItem;
        QTableWidgetItem *pItemDate=new QTableWidgetItem;

        if(status.bUpdate)
        {
            pItemName->setBackgroundColor(colDisabled);
            pItemInfo->setBackgroundColor(colDisabled);
            pItemVersion->setBackgroundColor(colDisabled);
            pItemDate->setBackgroundColor(colDisabled);
        }

        pItemName->setText(pMData->at(i).sName);
        pItemName->setData(Qt::UserRole,pMData->at(i).sName);       
        pTableWidget->setItem(i,CN_NAME,pItemName);

        pItemInfo->setText(pMData->at(i).sInfo);
        pTableWidget->setItem(i,CN_INFO,pItemInfo);

        pCheckBoxIs32->setEnabled(false);
        pCheckBoxIs32->setChecked(pMData->at(i).bIs32);
        pTableWidget->setCellWidget(i,CN_32,pCheckBoxIs32);

        pCheckBoxIs64->setEnabled(false);
        pCheckBoxIs64->setChecked(pMData->at(i).bIs64);
        pTableWidget->setCellWidget(i,CN_64,pCheckBoxIs64);

        pItemVersion->setText(pMData->at(i).sVersion);
        pTableWidget->setItem(i,CN_VERSION,pItemVersion);

        pItemDate->setText(pMData->at(i).sDate);
        pTableWidget->setItem(i,CN_DATE,pItemDate);

        if(status.bInstall||status.bUpdate)
        {
            QToolButton *pPushButtonInstall=new QToolButton(this);
            pPushButtonInstall->setProperty("Name",pMData->at(i).sName);
            connect(pPushButtonInstall,SIGNAL(clicked()),this,SLOT(installButtonSlot()));

            if(status.bInstall)
            {
                pPushButtonInstall->setText(tr("Install"));
            }
            else if(status.bUpdate)
            {
                pPushButtonInstall->setText(tr("Update"));
            }

            pTableWidget->setCellWidget(i,CN_INSTALL,pPushButtonInstall);
        }

        if(status.bRemove)
        {
            QToolButton *pPushButtonRemove=new QToolButton(this);
            pPushButtonRemove->setProperty("Name",pMData->at(i).sName);
            connect(pPushButtonRemove,SIGNAL(clicked()),this,SLOT(removeButtonSlot()));
            pPushButtonRemove->setText(tr("Remove"));
            pTableWidget->setCellWidget(i,CN_REMOVE,pPushButtonRemove);
        }
    }

    pTableWidget->setSortingEnabled(true);
}

void GuiMainWindow::createPlugin()
{
    DialogCreateModule dialogCreateModule(this);

    connect(&dialogCreateModule,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

    dialogCreateModule.exec();
}

void GuiMainWindow::aboutDialog()
{
    DialogAbout dialogAbout(this);

    dialogAbout.exec();
}

void GuiMainWindow::openPlugin()
{
    QString sInitDirectory; // TODO

    QString sFileName=QFileDialog::getOpenFileName(this,tr("Open plugin"),sInitDirectory,"*.x64dbg.zip");

    if(sFileName!="")
    {
        openPlugin(sFileName);
    }
}

void GuiMainWindow::optionsDialog()
{
    DialogOptions dialogOptions(this,&xOptions);

    dialogOptions.exec();

    xOptions.adjustStayOnTop(this);
}

void GuiMainWindow::exitProgram()
{
    this->close();
}

void GuiMainWindow::errorMessage(QString sMessage)
{
    QMessageBox::critical(this,tr("Error"),sMessage);
}

void GuiMainWindow::getModules()
{
    modulesData=Utils::getModulesData(xOptions.getDataPath());

    fillTable(ui->tableWidgetServerList,&(modulesData.listServerList),&(modulesData.mapStatus));
    fillTable(ui->tableWidgetInstalled,&(modulesData.listInstalled),&(modulesData.mapStatus));

    ui->pushButtonUpdateAllInstalledPlugins->setEnabled(modulesData.listUpdates.count());
}

void GuiMainWindow::openPlugin(QString sFileName)
{
    if(Utils::isPluginValid(sFileName))
    {
        DialogInstallModule dialogInstallModule(this,xOptions.getDataPath(),xOptions.getRootPath());
        connect(&dialogInstallModule,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

        dialogInstallModule.setFileName(sFileName);
        dialogInstallModule.exec();

        getModules();
    }
    else
    {
        errorMessage(QString("%1: %2").arg(tr("Invalid plugin file")).arg(sFileName));
    }
}

void GuiMainWindow::updateJsonList()
{
    QString sServerListFileName=Utils::getServerListFileName(xOptions.getDataPath());
    QString sServerLastestListFileName=Utils::getServerLastestListFileName(xOptions.getDataPath());

    Utils::WEB_RECORD record={};

    record.sFileName=sServerLastestListFileName;
    record.sLink=xOptions.getJson();

    DialogGetFileFromServerProcess dialogGetFileFromServer(this,QList<Utils::WEB_RECORD>()<<record);

    connect(&dialogGetFileFromServer,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

    dialogGetFileFromServer.exec();

    if(Utils::isGithubPresent(sServerLastestListFileName))
    {
        DialogUpdateGitProcess dialogUpdateGitProcess(this,sServerLastestListFileName);

        connect(&dialogUpdateGitProcess,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

        dialogUpdateGitProcess.exec();
    }

    Utils::updateServerList(sServerListFileName,sServerLastestListFileName);
}

void GuiMainWindow::installButtonSlot()
{
    QToolButton *pPushButton=qobject_cast<QToolButton *>(sender());
    QString sName=pPushButton->property("Name").toString();

    installPlugin(sName);
}

void GuiMainWindow::removeButtonSlot()
{
    QToolButton *pPushButton=qobject_cast<QToolButton *>(sender());
    QString sName=pPushButton->property("Name").toString();

    removePlugin(sName);
}

void GuiMainWindow::installPlugin(QString sName)
{
    if(sName!="")
    {
        Utils::MDATA mdata=Utils::getMDataByName(&(modulesData.listServerList),sName);

        if(mdata.sName!="")
        {
            DialogInstallModule dialogInstallModule(this,xOptions.getDataPath(),xOptions.getRootPath());
            connect(&dialogInstallModule,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

            if(dialogInstallModule.setMData(&mdata))
            {
                dialogInstallModule.exec();

                getModules();
            }
        }
    }
}

void GuiMainWindow::installPlugins(QList<QString> *pListNames)
{
    QList<QString> listFileNames;

    int nCount=pListNames->count();

    for(int i=0;i<nCount;i++)
    {
        Utils::MDATA mdata=Utils::getMDataByName(&(modulesData.listServerList),pListNames->at(i));

        if(mdata.sName!="")
        {
            QString sFileName=Utils::getModuleFileName(xOptions.getDataPath(),mdata.sName);

            if(XBinary::isFileHashValid(XBinary::HASH_SHA1,sFileName,mdata.sSHA1))
            {
                listFileNames.append(sFileName);
            }
        }
    }

    if(listFileNames.count())
    {
        DialogInstallModuleProcess dialogInstallModuleProcess(this,xOptions.getDataPath(),xOptions.getRootPath(),listFileNames);

        connect(&dialogInstallModuleProcess,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

        dialogInstallModuleProcess.exec();

        getModules();
    }
}

void GuiMainWindow::removePlugin(QString sName)
{
    if(sName!="")
    {
        DialogRemoveModule dialogRemoveModule(this,xOptions.getDataPath(),xOptions.getRootPath(),sName);

        connect(&dialogRemoveModule,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

        dialogRemoveModule.exec();

        getModules();
    }
}

void GuiMainWindow::infoPlugin(Utils::MDATA *pMData)
{
    if(pMData->sName!="")
    {
        DialogInfoModule dialogInfoModule(this,pMData);

        dialogInfoModule.exec();
    }
}

void GuiMainWindow::dragEnterEvent(QDragEnterEvent *pEvent)
{
    const QMimeData* mimeData=pEvent->mimeData();

    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList=mimeData->urls();

        if(urlList.count())
        {
            QString sFileName=urlList.at(0).toLocalFile();

            if(Utils::isPluginValid(sFileName))
            {
                pEvent->acceptProposedAction();
            }
        }
    }
}

void GuiMainWindow::dragMoveEvent(QDragMoveEvent *pEvent)
{
    pEvent->acceptProposedAction();
}

void GuiMainWindow::dropEvent(QDropEvent *pEvent)
{
    const QMimeData* mimeData=pEvent->mimeData();

    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList=mimeData->urls();

        if(urlList.count())
        {
            QString sFileName=urlList.at(0).toLocalFile();

            sFileName=XBinary::convertFileName(sFileName);

            openPlugin(sFileName);
        }
    }
}

void GuiMainWindow::on_tableWidgetServerList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->tableWidgetServerList->selectedItems().count())
    {
        QMenu contextMenu(this);

        QAction actionInfo(tr("Info"),this);
        connect(&actionInfo,SIGNAL(triggered()),this,SLOT(_infoPluginServerList()));

        QAction actionInstall(tr("Install"),this);
        connect(&actionInstall,SIGNAL(triggered()),this,SLOT(_installPluginServerList()));

        QAction actionUpdate(tr("Update"),this);
        connect(&actionUpdate,SIGNAL(triggered()),this,SLOT(_installPluginServerList()));

        QAction actionRemove(tr("Remove"),this);
        connect(&actionRemove,SIGNAL(triggered()),this,SLOT(_removePluginServerList()));

        contextMenu.addAction(&actionInfo);
        contextMenu.addSeparator();

        QString sName=ui->tableWidgetServerList->selectedItems().at(0)->data(Qt::UserRole).toString();

        Utils::STATUS status=modulesData.mapStatus.value(sName);

        if(status.bInstall)
        {
            contextMenu.addAction(&actionInstall);
        }

        if(status.bUpdate)
        {
            contextMenu.addAction(&actionUpdate);
        }

        if(status.bRemove)
        {
            contextMenu.addAction(&actionRemove);
        }

        contextMenu.exec(ui->tableWidgetServerList->viewport()->mapToGlobal(pos));
    }
}

void GuiMainWindow::on_tableWidgetInstalled_customContextMenuRequested(const QPoint &pos)
{
    if(ui->tableWidgetInstalled->selectedItems().count())
    {
        QMenu contextMenu(this);

        QAction actionInfo(tr("Info"),this);
        connect(&actionInfo,SIGNAL(triggered()),this,SLOT(_infoPluginInstalled()));

        QAction actionInstall(tr("Install"),this);
        connect(&actionInstall,SIGNAL(triggered()),this,SLOT(_installPluginInstalled()));

        QAction actionUpdate(tr("Update"),this);
        connect(&actionUpdate,SIGNAL(triggered()),this,SLOT(_installPluginInstalled()));

        QAction actionRemove(tr("Remove"),this);
        connect(&actionRemove,SIGNAL(triggered()),this,SLOT(_removePluginInstalled()));

        contextMenu.addAction(&actionInfo);
        contextMenu.addSeparator();

        QString sName=ui->tableWidgetInstalled->selectedItems().at(0)->data(Qt::UserRole).toString();

        Utils::STATUS status=modulesData.mapStatus.value(sName);

        if(status.bInstall)
        {
            contextMenu.addAction(&actionInstall);
        }

        if(status.bUpdate)
        {
            contextMenu.addAction(&actionUpdate);
        }

        if(status.bRemove)
        {
            contextMenu.addAction(&actionRemove);
        }

        contextMenu.exec(ui->tableWidgetInstalled->viewport()->mapToGlobal(pos));
    }
}

void GuiMainWindow::_infoPluginServerList()
{
    if(ui->tableWidgetServerList->selectedItems().count())
    {
        QString sName=ui->tableWidgetServerList->selectedItems().at(0)->data(Qt::UserRole).toString();

        Utils::MDATA mdata=Utils::getMDataByName(&(modulesData.listServerList),sName);

        infoPlugin(&mdata);
    }
}

void GuiMainWindow::_installPluginServerList()
{
    if(ui->tableWidgetServerList->selectedItems().count())
    {
        QString sName=ui->tableWidgetServerList->selectedItems().at(0)->data(Qt::UserRole).toString();

        installPlugin(sName);
    }
}

void GuiMainWindow::_removePluginServerList()
{
    if(ui->tableWidgetServerList->selectedItems().count())
    {
        QString sName=ui->tableWidgetServerList->selectedItems().at(0)->data(Qt::UserRole).toString();

        removePlugin(sName);
    }
}

void GuiMainWindow::_infoPluginInstalled()
{
    if(ui->tableWidgetInstalled->selectedItems().count())
    {
        QString sName=ui->tableWidgetInstalled->selectedItems().at(0)->data(Qt::UserRole).toString();

        Utils::MDATA mdata=Utils::getMDataByName(&(modulesData.listInstalled),sName);

        infoPlugin(&mdata);
    }
}

void GuiMainWindow::_installPluginInstalled()
{
    if(ui->tableWidgetInstalled->selectedItems().count())
    {
        QString sName=ui->tableWidgetInstalled->selectedItems().at(0)->data(Qt::UserRole).toString();

        installPlugin(sName);
    }
}

void GuiMainWindow::_removePluginInstalled()
{
    if(ui->tableWidgetInstalled->selectedItems().count())
    {
        QString sName=ui->tableWidgetInstalled->selectedItems().at(0)->data(Qt::UserRole).toString();

        removePlugin(sName);
    }
}

void GuiMainWindow::updateServerList()
{
    updateJsonList();

    getModules();
}

void GuiMainWindow::updateAllInstalledPlugins()
{
    DialogGetFileFromServerProcess dialogGetFileFromServer(this,modulesData.listUpdates);

    connect(&dialogGetFileFromServer,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

    dialogGetFileFromServer.exec();

    QList<QString> listNames=Utils::getNamesFromWebRecords(&modulesData.listUpdates);

    installPlugins(&listNames);
}

void GuiMainWindow::checkForUpdates()
{
    QNetworkAccessManager manager(this);
    QNetworkRequest request(QUrl(X_SERVERVERSION));
    QNetworkReply *pReply=manager.get(request);
    QEventLoop loop;
    QObject::connect(pReply,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();

    if(pReply->error()==QNetworkReply::NoError)
    {
        if(pReply->bytesAvailable())
        {
            QByteArray baData=pReply->readAll();
            QString sVersion=QString(baData.data());

            if(QCoreApplication::applicationVersion().toDouble()<sVersion.toDouble())
            {
                if(QMessageBox::information(this,tr("Update information"),
                                            QString("%1\r\n\r\n%2\r\n\r\n%3")
                                            .arg(tr("New version available"))
                                            .arg(sVersion)
                                            .arg(tr("Go to download page?")),
                                            QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes)
                {
                    QDesktopServices::openUrl(QUrl(X_HOMEPAGE));
                }
            }
            else
            {
                QMessageBox::information(this,tr("Update information"),tr("No update available"));
            }
        }
    }
    else
    {
        QMessageBox::critical(this,tr("Network error"),pReply->errorString());
    }

    delete pReply;
}

void GuiMainWindow::on_pushButtonUpdateServerList_clicked()
{
    updateServerList();
}

void GuiMainWindow::on_pushButtonUpdateAllInstalledPlugins_clicked()
{
    updateAllInstalledPlugins();
}

void GuiMainWindow::on_actionExit_triggered()
{
    exitProgram();
}

void GuiMainWindow::on_actionAbout_triggered()
{
    aboutDialog();
}

void GuiMainWindow::on_actionOpen_triggered()
{
    openPlugin();
}

void GuiMainWindow::on_actionCreate_triggered()
{
    createPlugin();
}

void GuiMainWindow::on_actionOptions_triggered()
{
    optionsDialog();
}

void GuiMainWindow::on_actionUpdate_server_list_triggered()
{
    updateServerList();
}

void GuiMainWindow::on_actionUpdate_all_installed_plugins_triggered()
{
    updateAllInstalledPlugins();
}

void GuiMainWindow::on_actionCheck_for_updates_triggered()
{
    checkForUpdates();
}
