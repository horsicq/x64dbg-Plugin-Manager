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
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "consoleoutput.h"
#include "xoptions.h"
#include "../global.h"
#include "../utils.h"
#include "../createmoduleprocess.h"
#include "../getfilefromserverprocess.h"
#include "../installmoduleprocess.h"
#include "../removemoduleprocess.h"
#include "../updategitprocess.h"
#include "../convertprocess.h"

enum PLGMNGREXITCODE
{
    PLGMNGREXITCODE_NOARGS=0,
    PLGMNGREXITCODE_ERROR,
    PLGMNGREXITCODE_DATAERROR,
    PLGMNGREXITCODE_INSTALLFILES,
    PLGMNGREXITCODE_INSTALLPLUGINS,
    PLGMNGREXITCODE_LISTNAMEISEMPTY,
    PLGMNGREXITCODE_NOINPUTFILES,
    PLGMNGREXITCODE_NOPLUGINSINSTALLED,
    PLGMNGREXITCODE_PLUGINCREATED,
    PLGMNGREXITCODE_CANNOTCREATESERVERLIST,
    PLGMNGREXITCODE_SHOWSERVERLIST,
    PLGMNGREXITCODE_SERVERLISTCREATED,
    PLGMNGREXITCODE_SERVERLISTISEMPTY,
    PLGMNGREXITCODE_SERVERLISTUPDATED,
    PLGMNGREXITCODE_SHOWINSTALLED,
    PLGMNGREXITCODE_SHOWUPDATES,
    PLGMNGREXITCODE_UPDATEALL,
    PLGMNGREXITCODE_UPDATEPLUGINS,
    PLGMNGREXITCODE_REMOVEPLUGINS,
    PLGMNGREXITCODE_NOUPDATESAVAILABLE,
};

void installFiles(QString sDataPath, QString sRootPath,ConsoleOutput *pConsoleOutput,QList<QString> *pListFileNames)
{
    int nCount=pListFileNames->count();

    for(int i=0;i<nCount;i++)
    {
        QString sFileName=pListFileNames->at(i);
        if(Utils::isPluginValid(sFileName))
        {
            InstallModuleProcess installModuleProcess;
            QObject::connect(&installModuleProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
            QObject::connect(&installModuleProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
            installModuleProcess.setData(sDataPath,sRootPath,QList<QString>()<<sFileName);
            installModuleProcess.process();
        }
        else
        {
            pConsoleOutput->errorMessage(QString("Invalid plugin file: %1").arg(sFileName));
        }
    }
}

void installModules(QString sDataPath, QString sRootPath,Utils::MODULES_DATA *pModulesData,ConsoleOutput *pConsoleOutput,QList<QString> *pListModuleNames)
{
    int nCount=pListModuleNames->count();

    for(int i=0;i<nCount;i++)
    {
        Utils::MDATA mdata=Utils::getMDataByName(&(pModulesData->listServerList),pListModuleNames->at(i));

        if(mdata.sName!="")
        {        
            QString sSHA1=mdata.sSHA1;

            QString sModuleFileName=Utils::getModuleFileName(sDataPath,mdata.sName);

            bool bHash=XBinary::isFileHashValid(XBinary::HASH_SHA1,sModuleFileName,sSHA1);

            if(!bHash)
            {
                if(mdata.sGithub!="")
                {
                    QString sConvertPath=Utils::getConvertPath(sDataPath,mdata.sName);
                    QString sDownloadModulePath=Utils::getConvertModulePath(sDataPath,mdata.sName);

                    XBinary::createDirectory(sConvertPath);
                    XBinary::createDirectory(sDownloadModulePath);

                    QList<Utils::WEB_RECORD> listWebRecords;

                    int nCount=mdata.listDownloads.count();

                    for(int i=0;i<nCount;i++)
                    {
                        Utils::WEB_RECORD record={};

                        QString sLink=mdata.listDownloads.at(i);

                        record.sFileName=sConvertPath+QDir::separator()+sLink.section("/",-1,-1);
                        record.sFileName=record.sFileName.remove("?raw=true");
                        record.sLink=sLink;

                        listWebRecords.append(record);
                    }

                    GetFileFromServerProcess getFileFromServerProcess;
                    QObject::connect(&getFileFromServerProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
                    QObject::connect(&getFileFromServerProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
                    getFileFromServerProcess.setData(listWebRecords);
                    getFileFromServerProcess.process();

                    ConvertProcess convertProcess;
                    QObject::connect(&convertProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
                    QObject::connect(&convertProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
                    convertProcess.setData(&mdata,sDataPath);
                    convertProcess.process();

                    Utils::MDATA _mdata=mdata;

                    _mdata.sBundleFileName=Utils::getModuleFileName(sDataPath,_mdata.sName);
                    _mdata.sRoot=sDownloadModulePath;

                    QString sErrorString;

                    if(Utils::checkMData(&_mdata,&sErrorString))
                    {
                        CreateModuleProcess createModuleProcess;
                        QObject::connect(&createModuleProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
                        QObject::connect(&createModuleProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
                        createModuleProcess.setData(&_mdata,false);
                        createModuleProcess.process();
                    }
                    else
                    {
                        pConsoleOutput->errorMessage(sErrorString);
                    }

                #ifndef QT_DEBUG
                    XBinary::removeDirectory(sConvertPath);
                #endif

                    Utils::updateJsonFile(Utils::getServerListFileName(sDataPath),QList<Utils::MDATA>() << _mdata);

                    sSHA1=_mdata.sSHA1;
                }
                else
                {
                    Utils::WEB_RECORD record={};

                    record.sFileName=sModuleFileName;
                    record.sLink=mdata.sSrc;

                    GetFileFromServerProcess getFileFromServerProcess;
                    QObject::connect(&getFileFromServerProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
                    QObject::connect(&getFileFromServerProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
                    getFileFromServerProcess.setData(QList<Utils::WEB_RECORD>()<<record);
                    getFileFromServerProcess.process();
                }

                bHash=XBinary::isFileHashValid(XBinary::HASH_SHA1,sModuleFileName,sSHA1);
            }

            if(bHash)
            {
                InstallModuleProcess installModuleProcess;
                QObject::connect(&installModuleProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
                QObject::connect(&installModuleProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
                installModuleProcess.setData(sDataPath,sRootPath,QList<QString>()<<sModuleFileName);
                installModuleProcess.process();
            }
            else
            {
                pConsoleOutput->errorMessage(QString("Invalid SHA1: %1").arg(sModuleFileName));
            }
        }
        else
        {
            pConsoleOutput->errorMessage(QString("Invalid name: %1").arg(pListModuleNames->at(i)));
        }
    }
}

void removeModules(QString sDataPath, QString sRootPath,Utils::MODULES_DATA *pModulesData,ConsoleOutput *pConsoleOutput,QList<QString> *pListModuleNames)
{
    int nCount=pListModuleNames->count();

    for(int i=0;i<nCount;i++)
    {
        Utils::MDATA mdata=Utils::getMDataByName(&(pModulesData->listInstalled),pListModuleNames->at(i));

        if(mdata.sName!="")
        {
            RemoveModuleProcess removeModuleProcess;
            QObject::connect(&removeModuleProcess,SIGNAL(infoMessage(QString)),pConsoleOutput,SLOT(infoMessage(QString)));
            QObject::connect(&removeModuleProcess,SIGNAL(errorMessage(QString)),pConsoleOutput,SLOT(errorMessage(QString)));
            removeModuleProcess.setData(sDataPath,sRootPath,QList<QString>()<<mdata.sName);
            removeModuleProcess.process();
        }
        else
        {
            pConsoleOutput->errorMessage(QString("Invalid name: %1. This module is not installed.").arg(pListModuleNames->at(i)));
        }
    }
}

void showModules(ConsoleOutput *pConsoleOutput,QList<Utils::MDATA> *pList)
{
    int nCount=pList->count();

    for(int i=0;i<nCount;i++)
    {
        QString sString=QString("%1 [%2]").arg(pList->at(i).sName).arg(pList->at(i).sVersion);

        pConsoleOutput->infoMessage(sString);
    }
}

int main(int argc, char *argv[])
{
    int nReturnCode=PLGMNGREXITCODE_NOARGS;

    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setOrganizationDomain(X_ORGANIZATIONDOMAIN);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(X_APPLICATIONVERSION);

    QCoreApplication app(argc, argv);

    XOptions xOptions;
    xOptions.setName(X_OPTIONSFILE);

    QList<XOptions::ID> listIDs;

    listIDs.append(XOptions::ID_DATAPATH);
    listIDs.append(XOptions::ID_ROOTPATH);
    listIDs.append(XOptions::ID_JSON);
    listIDs.append(XOptions::ID_AUTHUSER);
    listIDs.append(XOptions::ID_AUTHTOKEN);

    xOptions.setValueIDs(listIDs);

    QMap<XOptions::ID, QVariant> mapDefaultValues;

    mapDefaultValues.insert(XOptions::ID_JSON,X_JSON_DEFAULT);

    xOptions.setDefaultValues(mapDefaultValues);

    xOptions.load();

    ConsoleOutput consoleOutput;
    QCommandLineParser parser;
    QString sDescription;
    sDescription.append(QString("%1 v%2\n").arg(X_APPLICATIONNAME).arg(X_APPLICATIONVERSION));
    sDescription.append(QString("%1\n").arg("Copyright(C) 2019-2020 hors<horsicq@gmail.com> Web: http://ntinfo.biz"));
    parser.setApplicationDescription(sDescription);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("plugin or file","The plugin or file to open.");

    QCommandLineOption clSetGlobalRootPath          (QStringList()<<"G"<<"setglobalrootpath",   "Set a global root path<path>.",                "path");
    QCommandLineOption clSetGlobalDataPath          (QStringList()<<"D"<<"setglobaldatapath",   "Set a global data path<path>.",                "path");
    QCommandLineOption clSetGlobalJSONLink          (QStringList()<<"J"<<"setglobaljsonlink",   "Set a global JSON link<link>.",                "link");
    QCommandLineOption clCreatePlugin               (QStringList()<<"c"<<"createplugin",        "Create a plugin<name>.",                       "name");
    QCommandLineOption clCreateServerList           (QStringList()<<"l"<<"createserverlist",    "Create a serverlist<name>.",                   "name");
    QCommandLineOption clInstallPlugin              (QStringList()<<"i"<<"installplugin",       "Install plugin(s)."                            );
    QCommandLineOption clInstallFile                (QStringList()<<"f"<<"installfile",         "Install file(s)."                              );
    QCommandLineOption clUpdatePlugin               (QStringList()<<"u"<<"updateplugin",        "Update plugin(s)."                             );
    QCommandLineOption clRemovePlugin               (QStringList()<<"m"<<"removeplugin",        "Remove plugin(s)."                             );
    QCommandLineOption clUpdateServerList           (QStringList()<<"U"<<"updateserverlist",    "Update server list."                           );
    QCommandLineOption clUpdateAllInstalledPlugins  (QStringList()<<"A"<<"updateall",           "Update all installed plugins."                 );
    QCommandLineOption clShowServerList             (QStringList()<<"S"<<"showserverlist",      "Show server list."                             );
    QCommandLineOption clShowInstalled              (QStringList()<<"N"<<"showinstalled",       "Show installed."                               );
    QCommandLineOption clShowUpdates                (QStringList()<<"P"<<"showupdates",         "Show updates."                                 );
    QCommandLineOption clSetRootPath                (QStringList()<<"r"<<"setrootpath",         "Set a root path<path>.",                       "path");
    QCommandLineOption clSetName                    (QStringList()<<"n"<<"setname",             "Set a name of plugin<name>.",                  "name");
    QCommandLineOption clSetVersion                 (QStringList()<<"V"<<"setversion",          "Set a version of plugin<version>.",            "version");
    QCommandLineOption clSetDate                    (QStringList()<<"d"<<"setdate",             "Set a date of plugin<date>.",                  "date");
    QCommandLineOption clSetAuthor                  (QStringList()<<"a"<<"setauthor",           "Set an author of plugin<author>.",             "author");
    QCommandLineOption clSetBugreport               (QStringList()<<"b"<<"setbugreport",        "Set a bugreport of plugin<bugreport>.",        "bugreport");
    QCommandLineOption clSetInfo                    (QStringList()<<"I"<<"setinfo",             "Set an info of plugin<info>.",                 "info");
    QCommandLineOption clSetWebPrefix               (QStringList()<<"p"<<"setwebprefix",        "Set a webprefix<prefix>.",                     "prefix");

    parser.addOption(clSetGlobalRootPath);
    parser.addOption(clSetGlobalDataPath);
    parser.addOption(clSetGlobalJSONLink);
    parser.addOption(clCreatePlugin);
    parser.addOption(clCreateServerList);
    parser.addOption(clInstallPlugin);
    parser.addOption(clInstallFile);
    parser.addOption(clUpdatePlugin);
    parser.addOption(clRemovePlugin);
    parser.addOption(clUpdateServerList);
    parser.addOption(clUpdateAllInstalledPlugins);
    parser.addOption(clShowServerList);
    parser.addOption(clShowInstalled);
    parser.addOption(clShowUpdates);
    parser.addOption(clSetRootPath);
    parser.addOption(clSetName);
    parser.addOption(clSetVersion);
    parser.addOption(clSetDate);
    parser.addOption(clSetAuthor);
    parser.addOption(clSetBugreport);
    parser.addOption(clSetInfo);
    parser.addOption(clSetWebPrefix);

    parser.process(app);

    bool bProcess=false;

    bool bIsSetGlobalRootPath=parser.isSet(clSetGlobalRootPath);
    bool bIsSetGlobalDataPath=parser.isSet(clSetGlobalDataPath);
    bool bIsSetGlobalJSONLink=parser.isSet(clSetGlobalJSONLink);

    if(bIsSetGlobalRootPath||bIsSetGlobalDataPath||bIsSetGlobalJSONLink)
    {
        bProcess=true;

        if(bIsSetGlobalRootPath)
        {
            xOptions.setValue(XOptions::ID_ROOTPATH,parser.value(clSetGlobalRootPath));

            if(xOptions.getRootPath()!="")
            {
                XBinary::createDirectory(XBinary::convertPathName(xOptions.getRootPath()));
            }

            if(XBinary::isDirectoryExists(XBinary::convertPathName(xOptions.getRootPath())))
            {
                consoleOutput.infoMessage(QString("Set a global root path: %1").arg(xOptions.getRootPath()));
            }
            else
            {
                xOptions.clearValue(XOptions::ID_ROOTPATH);
                consoleOutput.errorMessage(QString("Invalid root path: %1").arg(xOptions.getRootPath()));
            }
        }
        if(bIsSetGlobalDataPath)
        {
            xOptions.setValue(XOptions::ID_DATAPATH,parser.value(clSetGlobalDataPath));

            if(xOptions.getDataPath()!="")
            {
                XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath()));
            }

            if(XBinary::isDirectoryExists(XBinary::convertPathName(xOptions.getDataPath())))
            {
                consoleOutput.infoMessage(QString("Set a global data path: %1").arg(xOptions.getDataPath()));
            }
            else
            {
                xOptions.clearValue(XOptions::ID_DATAPATH);
                consoleOutput.errorMessage(QString("Invalid data path: %1").arg(xOptions.getDataPath()));
            }
        }
        if(bIsSetGlobalJSONLink)
        {
            xOptions.setValue(XOptions::ID_JSON,parser.value(clSetGlobalJSONLink));
            consoleOutput.infoMessage(QString("Set a global JSON link: %1").arg(xOptions.getJson()));
        }

        xOptions.save();
    }

    if(parser.isSet(clSetRootPath))
    {
        xOptions.setValue(XOptions::ID_ROOTPATH,parser.value(clSetRootPath));
    }

    bool bRootPathPresent=false;
    bool bDataPathPresent=false;

    if(xOptions.getRootPath()!="")
    {
        XBinary::createDirectory(XBinary::convertPathName(xOptions.getRootPath()));
        bRootPathPresent=XBinary::isDirectoryExists(XBinary::convertPathName(xOptions.getRootPath()));
    }

    if(xOptions.getDataPath()!="")
    {
        XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath()));
        XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath())+QDir::separator()+"installed");
        XBinary::createDirectory(XBinary::convertPathName(xOptions.getDataPath())+QDir::separator()+"modules");
        bDataPathPresent=XBinary::isDirectoryExists(XBinary::convertPathName(xOptions.getDataPath()));
    }

    if(!bRootPathPresent)
    {
        consoleOutput.errorMessage(QString("Invalid root path: %1").arg(xOptions.getRootPath()));
    }

    if(!bDataPathPresent)
    {
        consoleOutput.errorMessage(QString("Invalid data path: %1").arg(xOptions.getDataPath()));
    }

    if(parser.isSet(clCreatePlugin))
    {
        bProcess=true;

        Utils::MDATA mdata={};

        mdata.sBundleFileName=parser.value(clCreatePlugin);
        consoleOutput.infoMessage(QString("Create a plugin: %1").arg(mdata.sBundleFileName));

        if(mdata.sBundleFileName!="")
        {
            mdata.sBundleFileName+=".x64dbg.zip";
        }

        mdata.sRoot=parser.value(clSetRootPath);
        mdata.sName=parser.value(clSetName);
        mdata.sVersion=parser.value(clSetVersion);
        mdata.sDate=parser.value(clSetDate);
        mdata.sAuthor=parser.value(clSetAuthor);
        mdata.sBugreport=parser.value(clSetBugreport);
        mdata.sInfo=parser.value(clSetInfo);

        QString sErrorString;

        if(Utils::checkMData(&mdata,&sErrorString))
        {
            CreateModuleProcess createModuleProcess;
            QObject::connect(&createModuleProcess,SIGNAL(infoMessage(QString)),&consoleOutput,SLOT(infoMessage(QString)));
            QObject::connect(&createModuleProcess,SIGNAL(errorMessage(QString)),&consoleOutput,SLOT(errorMessage(QString)));
            createModuleProcess.setData(&mdata,true);
            createModuleProcess.process();

            nReturnCode=PLGMNGREXITCODE_PLUGINCREATED;
        }
        else
        {
            consoleOutput.errorMessage(sErrorString);

            nReturnCode=PLGMNGREXITCODE_DATAERROR;
        }
    }
    else if(parser.isSet(clCreateServerList))
    {
        bProcess=true;

        QString sListName=parser.value(clCreateServerList);
        consoleOutput.infoMessage(QString("Create a serverlist: %1").arg(sListName));
        QString sWebPrefix=parser.value(clSetWebPrefix);
        QString sDate=parser.value(clSetDate);

        if(sDate=="")
        {
            sDate=QDate::currentDate().toString("yyyy-MM-dd");
        }

        if(sListName!="")
        {
            QList<QString> listFiles=parser.positionalArguments();

            if(listFiles.count())
            {
                if(Utils::createServerList(sListName,&listFiles,sWebPrefix,sDate))
                {
                    nReturnCode=PLGMNGREXITCODE_SERVERLISTCREATED;
                }
                else
                {
                    consoleOutput.errorMessage("Cannot create serverlist.");

                    nReturnCode=PLGMNGREXITCODE_CANNOTCREATESERVERLIST;
                }
            }
            else
            {
                consoleOutput.errorMessage("No input files.");

                nReturnCode=PLGMNGREXITCODE_NOINPUTFILES;
            }
        }
        else
        {
            consoleOutput.errorMessage("List name is empty.");

            nReturnCode=PLGMNGREXITCODE_LISTNAMEISEMPTY;
        }
    }
    else if(parser.isSet(clUpdateServerList))
    {
        bProcess=true;

        consoleOutput.infoMessage(QString("Update server list."));

        QString sServerListFileName=Utils::getServerListFileName(xOptions.getDataPath());
        QString sServerLastestListFileName=Utils::getServerLastestListFileName(xOptions.getDataPath());

        Utils::WEB_RECORD record={};

        record.sFileName=sServerLastestListFileName;
        record.sLink=xOptions.getJson();

        GetFileFromServerProcess getFileFromServerProcess;
        QObject::connect(&getFileFromServerProcess,SIGNAL(infoMessage(QString)),&consoleOutput,SLOT(infoMessage(QString)));
        QObject::connect(&getFileFromServerProcess,SIGNAL(errorMessage(QString)),&consoleOutput,SLOT(errorMessage(QString)));
        getFileFromServerProcess.setData(QList<Utils::WEB_RECORD>()<<record);

        getFileFromServerProcess.process();

        if(Utils::isGithubPresent(sServerLastestListFileName))
        {
            UpdateGitProcess updateGitProcess;

            QString authUser = xOptions.getValue(XOptions::ID_AUTHUSER).toString();
            if(!authUser.isEmpty())
            {
                QString authToken = xOptions.getValue(XOptions::ID_AUTHTOKEN).toString();
                updateGitProcess.setCredentials(authUser, authToken);
            }

            QObject::connect(&updateGitProcess,SIGNAL(infoMessage(QString)),&consoleOutput,SLOT(infoMessage(QString)));
            QObject::connect(&updateGitProcess,SIGNAL(errorMessage(QString)),&consoleOutput,SLOT(errorMessage(QString)));
            updateGitProcess.setData(sServerLastestListFileName);
            updateGitProcess.process();
        }

        Utils::updateServerList(sServerListFileName,sServerLastestListFileName);

        nReturnCode=PLGMNGREXITCODE_SERVERLISTUPDATED;
    }
    else if(parser.isSet(clInstallPlugin)||
            parser.isSet(clInstallFile)||
            parser.isSet(clUpdatePlugin)||
            parser.isSet(clRemovePlugin)||
            parser.isSet(clUpdateAllInstalledPlugins)||
            parser.isSet(clShowServerList)||
            parser.isSet(clShowInstalled)||
            parser.isSet(clShowUpdates))
    {
        bProcess=true;

        if(bRootPathPresent&&bDataPathPresent)
        {
            Utils::MODULES_DATA modulesData=Utils::getModulesData(xOptions.getDataPath());

            if(parser.isSet(clShowServerList))
            {
                if(modulesData.listServerList.count())
                {
                    consoleOutput.infoMessage(QString("Show server list."));

                    showModules(&consoleOutput,&modulesData.listServerList);

                    nReturnCode=PLGMNGREXITCODE_SHOWSERVERLIST;
                }
                else
                {
                    consoleOutput.infoMessage(QString("Server list is empty. Please update server list('-U' or '--updateserverlist')"));

                    nReturnCode=PLGMNGREXITCODE_SERVERLISTISEMPTY;
                }
            }
            else if(parser.isSet(clShowInstalled))
            {
                if(modulesData.listInstalled.count())
                {
                    consoleOutput.infoMessage(QString("Show installed."));

                    showModules(&consoleOutput,&modulesData.listInstalled);

                    nReturnCode=PLGMNGREXITCODE_SHOWINSTALLED;
                }
                else
                {
                    consoleOutput.infoMessage(QString("No plugins installed"));

                    nReturnCode=PLGMNGREXITCODE_NOPLUGINSINSTALLED;
                }
            }
            else if(parser.isSet(clShowUpdates))
            {
                if(modulesData.listUpdates.count())
                {
                    consoleOutput.infoMessage(QString("Show updates."));

                    int nCount=modulesData.listUpdates.count();

                    for(int i=0;i<nCount;i++)
                    {
                        consoleOutput.infoMessage(modulesData.listUpdates.at(i).sName);
                    }

                    nReturnCode=PLGMNGREXITCODE_SHOWUPDATES;
                }
                else
                {
                    consoleOutput.infoMessage(QString("No updates available."));

                    nReturnCode=PLGMNGREXITCODE_NOUPDATESAVAILABLE;
                }
            }
            else if(parser.isSet(clInstallPlugin))
            {
                consoleOutput.infoMessage(QString("Install plugin(s)."));

                QList<QString> listModules=parser.positionalArguments();

                installModules(xOptions.getDataPath(),xOptions.getRootPath(),&modulesData,&consoleOutput,&listModules);

                nReturnCode=PLGMNGREXITCODE_INSTALLPLUGINS;
            }
            else if(parser.isSet(clInstallFile))
            {
                consoleOutput.infoMessage(QString("Install file(s)."));

                QList<QString> listFiles=parser.positionalArguments();

                installFiles(xOptions.getDataPath(),xOptions.getRootPath(),&consoleOutput,&listFiles);

                nReturnCode=PLGMNGREXITCODE_INSTALLFILES;
            }
            else if(parser.isSet(clUpdatePlugin))
            {
                consoleOutput.infoMessage(QString("Update plugin(s)."));

                QList<QString> listModules=parser.positionalArguments();
                QList<QString> _listModules;

                int nCount=listModules.count();

                for(int i=0;i<nCount;i++)
                {
                    Utils::WEB_RECORD webRecord=Utils::getWebRecordByName(&(modulesData.listUpdates),listModules.at(i));

                    if(webRecord.sName!="")
                    {
                        _listModules.append(webRecord.sName);
                    }
                    else
                    {
                         consoleOutput.errorMessage(QString("No update available for module: %1").arg(listModules.at(i)));
                    }
                }

                installModules(xOptions.getDataPath(),xOptions.getRootPath(),&modulesData,&consoleOutput,&_listModules);

                nReturnCode=PLGMNGREXITCODE_UPDATEPLUGINS;
            }
            else if(parser.isSet(clRemovePlugin))
            {
                consoleOutput.infoMessage(QString("Remove plugin(s)."));

                QList<QString> listModules=parser.positionalArguments();

                removeModules(xOptions.getDataPath(),xOptions.getRootPath(),&modulesData,&consoleOutput,&listModules);

                nReturnCode=PLGMNGREXITCODE_REMOVEPLUGINS;
            }
            else if(parser.isSet(clUpdateAllInstalledPlugins))
            {
                if(modulesData.listUpdates.count())
                {
                    consoleOutput.infoMessage(QString("Update all installed plugins"));

                    QList<QString> listModules=Utils::getNamesFromWebRecords(&(modulesData.listUpdates));

                    installModules(xOptions.getDataPath(),xOptions.getRootPath(),&modulesData,&consoleOutput,&listModules);

                    nReturnCode=PLGMNGREXITCODE_UPDATEALL;
                }
                else
                {
                    consoleOutput.infoMessage(QString("No updates available."));

                    nReturnCode=PLGMNGREXITCODE_NOUPDATESAVAILABLE;
                }
            }
        }
    }

    if(!bProcess)
    {
        parser.showHelp();
        Q_UNREACHABLE();
    }

    return nReturnCode;
}
