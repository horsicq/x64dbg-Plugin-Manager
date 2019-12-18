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

    int nCount=listInstalledModules.count();
    // TODO
    // Load list from installed
    // If empty make request
    // list.json -> main list

    // TODO modules from List
}

void GuiMainWindow::openPlugin(QString sFileName)
{
    QFile file;

    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        if(Utils::isPluginValid(&file))
        {
            DialogInstallModule dialogInstallModule(this,&options,&file);

            dialogInstallModule.exec();
        }
        else
        {
            errorMessage(tr("Invalid plugin file"));
        }

        file.close();
    }
}

void GuiMainWindow::updateJsonList()
{
    DialogGetFileFromServer dialogGetFileFromServer(this,options.sJSONFile,XBinary::convertPathName(options.sDataPath)+QDir::separator()+"list.json");

    dialogGetFileFromServer.exec();
}
