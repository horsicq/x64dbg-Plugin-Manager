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
#include "guimainwindow.h"
#include "ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString("%1 v%2").arg(X_APPLICATIONNAME).arg(X_APPLICATIONVERSION));

    DialogOptions::loadOptions(&options);

    if(!XBinary::isDirectoryExists(XBinary::convertPathName(options.sRootPath)))
    {
        options.sRootPath="";
    }

    if(options.sRootPath=="")
    {
        QMessageBox::information(this,tr("Information"),tr("Please select a root directory for x64dbg"));

        QString sDirectoryName=QFileDialog::getExistingDirectory(this,tr("Select root directory"));

        if(sDirectoryName!="")
        {
            options.sRootPath=sDirectoryName;
        }
        else
        {
            exit(1);
        }
    }

    if(options.sRootPath=="")
    {
        exit(1);
    }

    XBinary::createDirectory(XBinary::convertPathName(options.sDataPath));
    XBinary::createDirectory(XBinary::convertPathName(options.sDataPath)+QDir::separator()+"installed");
    XBinary::createDirectory(XBinary::convertPathName(options.sDataPath)+QDir::separator()+"modules");

    setAcceptDrops(true);

    if(QCoreApplication::arguments().count()>1)
    {
        QString sFileName=QCoreApplication::arguments().at(1);

        openPlugin(sFileName);
    }
    else
    {
        if(!XBinary::isFileExists(XBinary::convertPathName(options.sDataPath)+QDir::separator()+"list.json"))
        {
            updateJsonList();
        }
    }

    ui->tableWidgetPlugins->setColumnCount(CN_size);
    ui->tableWidgetPlugins->setRowCount(0);

    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_NAME,            new QTableWidgetItem(tr("Name")));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_INFO,            new QTableWidgetItem(tr("Information")));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_32,              new QTableWidgetItem(tr("32")));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_64,              new QTableWidgetItem(tr("64")));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_CURRENTVERSION,  new QTableWidgetItem(tr("Current")));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_LASTVERSION,     new QTableWidgetItem(tr("Last")));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_INSTALL,         new QTableWidgetItem(""));
    ui->tableWidgetPlugins->setHorizontalHeaderItem(CN_REMOVE,          new QTableWidgetItem(""));

    ui->tableWidgetPlugins->setColumnWidth(CN_NAME,                     100);
    ui->tableWidgetPlugins->setColumnWidth(CN_INFO,                     300);
    ui->tableWidgetPlugins->setColumnWidth(CN_32,                       10);
    ui->tableWidgetPlugins->setColumnWidth(CN_64,                       10);
    ui->tableWidgetPlugins->setColumnWidth(CN_CURRENTVERSION,           80);
    ui->tableWidgetPlugins->setColumnWidth(CN_LASTVERSION,              80);
    ui->tableWidgetPlugins->setColumnWidth(CN_INSTALL,                  60);
    ui->tableWidgetPlugins->setColumnWidth(CN_REMOVE,                   60);

    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_NAME,           QHeaderView::Interactive);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_INFO,           QHeaderView::Stretch);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_32,             QHeaderView::ResizeToContents);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_64,             QHeaderView::ResizeToContents);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_CURRENTVERSION, QHeaderView::Interactive);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_LASTVERSION,    QHeaderView::Interactive);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_INSTALL,        QHeaderView::Interactive);
    ui->tableWidgetPlugins->horizontalHeader()->setSectionResizeMode(CN_REMOVE,         QHeaderView::Interactive);

    getModules();
}

GuiMainWindow::~GuiMainWindow()
{
    DialogOptions::saveOptions(&options);

    delete ui;
}

void GuiMainWindow::on_actionCreate_triggered()
{
    DialogCreateModule dialogCreateModule(this);

    connect(&dialogCreateModule,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));

    dialogCreateModule.exec();
}

void GuiMainWindow::on_pushButtonUpdateAll_clicked()
{
    // TODO
}

void GuiMainWindow::on_actionAbout_triggered()
{
    DialogAbout dialogAbout(this);

    dialogAbout.exec();
}

void GuiMainWindow::on_actionOpen_triggered()
{
    QString sInitDirectory; // TODO

    QString sFileName=QFileDialog::getOpenFileName(this,tr("Open plugin"),sInitDirectory,"*.x64dbg.zip");

    if(sFileName!="")
    {
        openPlugin(sFileName);
    }
}

void GuiMainWindow::on_actionOptions_triggered()
{
    DialogOptions dialogOptions(this,&options);

    dialogOptions.exec();

    Qt::WindowFlags wf=windowFlags();

    if(options.bStayOnTop)
    {
        wf|=Qt::WindowStaysOnTopHint;
    }
    else
    {
        wf&=~(Qt::WindowStaysOnTopHint);
    }

    setWindowFlags(wf);

    show();
}

void GuiMainWindow::on_pushButtonReload_clicked()
{
    updateJsonList();

    getModules();
}

void GuiMainWindow::on_actionExit_triggered()
{
    this->close();
}

void GuiMainWindow::errorMessage(QString sMessage)
{
    QMessageBox::critical(this,tr("Error"),sMessage);
}

void GuiMainWindow::getModules()
{
    QList<Utils::MDATA> listInstalledModules=Utils::getInstalledModules(XBinary::convertPathName(options.sDataPath),XBinary::convertPathName(options.sRootPath));
    QList<Utils::MDATA> listModulesFromJSON=Utils::getModulesFromJSONFile(XBinary::convertPathName(options.sDataPath)+QDir::separator()+"list.json");
    _listModules=Utils::mergeMData(&listInstalledModules,&listModulesFromJSON);

    int nCount=_listModules.count();

    ui->tableWidgetPlugins->setRowCount(nCount);

    for(int i=0;i<nCount;i++)
    {
        QTableWidgetItem *pItemName=new QTableWidgetItem;
        pItemName->setText(_listModules.at(i).sName);
        pItemName->setData(Qt::UserRole,i);
        ui->tableWidgetPlugins->setItem(i,CN_NAME,pItemName);

        QTableWidgetItem *pItemInfo=new QTableWidgetItem;
        pItemInfo->setText(_listModules.at(i).sInfo);
        ui->tableWidgetPlugins->setItem(i,CN_INFO,pItemInfo);

        QCheckBox *pCheckBoxIs32=new QCheckBox(this);
        pCheckBoxIs32->setEnabled(false);
        pCheckBoxIs32->setChecked(true);
        ui->tableWidgetPlugins->setCellWidget(i,CN_32,pCheckBoxIs32);

        QCheckBox *pCheckBoxIs64=new QCheckBox(this);
        pCheckBoxIs64->setEnabled(false);
        pCheckBoxIs64->setChecked(true);
        ui->tableWidgetPlugins->setCellWidget(i,CN_64,pCheckBoxIs64);

        QTableWidgetItem *pItemCurrentVersion=new QTableWidgetItem;
        pItemCurrentVersion->setText(_listModules.at(i).sCurrentVersion);
        ui->tableWidgetPlugins->setItem(i,CN_CURRENTVERSION,pItemCurrentVersion);

        QTableWidgetItem *pItemLastVersion=new QTableWidgetItem;
        pItemLastVersion->setText(_listModules.at(i).sLastVersion);
        ui->tableWidgetPlugins->setItem(i,CN_LASTVERSION,pItemLastVersion);

        QToolButton *pPushButtonInstall=new QToolButton(this);
        pPushButtonInstall->setProperty("ID",i);
        connect(pPushButtonInstall,SIGNAL(clicked()),this,SLOT(installButtonSlot()));
        pPushButtonInstall->setText(tr("Install"));
        ui->tableWidgetPlugins->setCellWidget(i,CN_INSTALL,pPushButtonInstall);

        QToolButton *pPushButtonRemove=new QToolButton(this);
        pPushButtonRemove->setProperty("ID",i);
        connect(pPushButtonRemove,SIGNAL(clicked()),this,SLOT(removeButtonSlot()));
        pPushButtonRemove->setText(tr("Remove"));
        ui->tableWidgetPlugins->setCellWidget(i,CN_REMOVE,pPushButtonRemove);
    }
}

void GuiMainWindow::openPlugin(QString sFileName)
{
    if(Utils::isPluginValid(sFileName))
    {
        QFileInfo fi(sFileName);
        QString sDestFile=XBinary::convertPathName(options.sDataPath)+QDir::separator()+"modules"+QDir::separator()+fi.fileName();

        if(XBinary::isFileExists(sDestFile))
        {
            XBinary::removeFile(sDestFile); // TODO handle errors
        }

        if(XBinary::copyFile(sFileName,sDestFile))
        {
            DialogInstallModule dialogInstallModule(this,&options,sDestFile);

            dialogInstallModule.exec();

            getModules();
        }
    }
    else
    {
        errorMessage(tr("Invalid plugin file"));
    }
}

void GuiMainWindow::updateJsonList()
{
    DialogGetFileFromServer dialogGetFileFromServer(this,options.sJSONFile,XBinary::convertPathName(options.sDataPath)+QDir::separator()+"list.json");

    dialogGetFileFromServer.exec();
}

void GuiMainWindow::installButtonSlot()
{
    QToolButton *pPushButton=qobject_cast<QToolButton *>(sender());
    qint32 nID=pPushButton->property("ID").toInt();

    if(nID<_listModules.count())
    {
        Utils::MDATA mdata=_listModules.at(nID);

        // TODO download

//        DialogInstallModule dialogInstallModule(this,mdata.sName);

//        dialogRemoveModule.exec();
    }
}

void GuiMainWindow::removeButtonSlot()
{
    QToolButton *pPushButton=qobject_cast<QToolButton *>(sender());
    qint32 nID=pPushButton->property("ID").toInt();

    if(nID<_listModules.count())
    {
        Utils::MDATA mdata=_listModules.at(nID);

        DialogRemoveModule dialogRemoveModule(this,mdata.sName);

        dialogRemoveModule.exec();
    }
}
