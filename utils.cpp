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
#include "utils.h"

Utils::Utils(QObject *parent) : QObject(parent)
{

}

void Utils::loadOptions(XPLUGINMANAGER::OPTIONS *pOptions)
{
    QSettings settings(QCoreApplication::applicationDirPath()+QDir::separator()+"x64plgmnr.ini",QSettings::IniFormat);

    pOptions->bStayOnTop=settings.value("StayOnTop",false).toBool();
    pOptions->sRootPath=settings.value("RootPath","").toString(); // TODO
    pOptions->sDataPath=settings.value("DataPath","$app/data").toString();
    pOptions->sJSONLink=settings.value("JSONFile",X_JSON_DEFAULT).toString();
}

void Utils::saveOptions(XPLUGINMANAGER::OPTIONS *pOptions)
{
    QSettings settings(QCoreApplication::applicationDirPath()+QDir::separator()+"x64plgmnr.ini",QSettings::IniFormat);

    settings.setValue("StayOnTop",pOptions->bStayOnTop);
    settings.setValue("RootPath",pOptions->sRootPath);
    settings.setValue("DataPath",pOptions->sDataPath);
    settings.setValue("JSONFile",pOptions->sJSONLink);
}

QList<Utils::RECORD> Utils::getRecords(QString sRootPath)
{
    QList<Utils::RECORD> listResult;

    QFileInfo fi(sRootPath);

    sRootPath=fi.absoluteDir().path()+QDir::separator()+fi.fileName();

    _getRecords(sRootPath,sRootPath,&listResult);

    return listResult;
}

bool Utils::checkMData(Utils::MDATA *pMData, QString *psErrorString)
{
    bool bResult=true;

    if(bResult&&(pMData->sName==""))
    {
        *psErrorString=tr("Invalid name");
        bResult=false;
    }

    if(bResult&&(pMData->sBundleFileName==""))
    {
        *psErrorString=tr("Invalid bundle file name");
        bResult=false;
    }

    if(bResult&&(pMData->sRoot==""))
    {
        if(!XBinary::isDirectoryExists(XBinary::convertPathName(pMData->sRoot)))
        {
            *psErrorString=tr("Invalid root path");
            bResult=false;
        }
    }

    return bResult;
}

QString Utils::createBundleName(Utils::MDATA *pMData)
{
    QString sResult;

    sResult+=pMData->sName;

    if(pMData->sVersion!="")
    {
        sResult+=QString("_%1").arg(pMData->sVersion);
    }

    return sResult;
}

bool Utils::isPluginValid(QString sFileName)
{
    bool bResult=false;

    QFile file;

    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        bResult=isPluginValid(&file);

        file.close();
    }

    return bResult;
}

bool Utils::isPluginValid(QIODevice *pDevice)
{
    bool bResult=false;

    XZip xzip(pDevice);

    if(xzip.isValid())
    {
        QList<XArchive::RECORD> listRecords=xzip.getRecords();

        bResult=XArchive::isArchiveRecordPresent("plugin_info.json",&listRecords);
    }

    return bResult;
}

QByteArray Utils::createPluginInfo(Utils::MDATA *pMData, QList<Utils::FILE_RECORD> *pListFileRecords, QList<Utils::DIRECTORY_RECORD> *pListDirectoryRecords)
{
    QByteArray baResult;

    QJsonObject recordObject;

    mDataToObject(pMData,&recordObject);

    if(pMData->sSHA1=="") // In zip
    {
        int nFilesCount=pListFileRecords->count();
        int nDirectoriesCount=pListDirectoryRecords->count();

        QJsonArray installArray;

        for(int i=0;i<nDirectoriesCount;i++)
        {
            QJsonObject record;

            record.insert("Path",       pListDirectoryRecords->at(i).sPath);
            record.insert("Action",     "make_directory");

            installArray.append(record);
        }

        for(int i=0;i<nFilesCount;i++)
        {
            QJsonObject record;

            record.insert("Path",       pListFileRecords->at(i).sPath);
            record.insert("Action",     "copy_file");
            record.insert("SHA1",       pListFileRecords->at(i).sSHA1);

            installArray.append(record);
        }

        recordObject.insert("Install",  installArray);

        QJsonArray removeArray;

        for(int i=0;i<nFilesCount;i++)
        {
            QJsonObject record;

            record.insert("Path",       pListFileRecords->at(i).sPath);
            record.insert("Action",     "remove_file");

            removeArray.append(record);
        }

        for(int i=nDirectoriesCount-1;i>=0;i--)
        {
            QJsonObject record;

            record.insert("Path",       pListDirectoryRecords->at(i).sPath);
            record.insert("Action",     "remove_directory_if_empty");

            removeArray.append(record);
        }

        recordObject.insert("Remove",   removeArray);
    }

    QJsonDocument doc(recordObject);
    baResult.append(doc.toJson());

    return baResult;
}

Utils::MDATA Utils::getMDataFromZip(QString sFileName, QString sRootPath)
{
    Utils::MDATA result={};

    QFile file;

    file.setFileName(sFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        result=getMDataFromZip(&file,sRootPath);

        file.close();
    }

    return result;
}

Utils::MDATA Utils::getMDataFromZip(QIODevice *pDevice, QString sRootPath)
{
    Utils::MDATA result={};

    result.sRoot=sRootPath;

    XZip xzip(pDevice);

    if(xzip.isValid())
    {
        QList<XArchive::RECORD> listRecords=xzip.getRecords();

        XArchive::RECORD pluginInfoRecord=XArchive::getArchiveRecord("plugin_info.json",&listRecords);

        QByteArray baData=xzip.decompress(&pluginInfoRecord);

        result=getMDataFromData(baData,sRootPath);
    }

    return result;
}

Utils::MDATA Utils::getMDataFromData(QByteArray baData, QString sRootPath)
{
    Utils::MDATA result={};

    QJsonDocument jsDoc=QJsonDocument::fromJson(baData);

    QJsonObject rootObj=jsDoc.object();

    objectToMData(&rootObj,&result);

    QJsonArray installArray=rootObj.value("Install").toArray();
    QJsonArray removeArray=rootObj.value("Remove").toArray();

    int nInstallCount=installArray.count();

    for(int i=0;i<nInstallCount;i++)
    {
        QJsonObject recordObj=installArray.at(i).toObject();
        INSTALL_RECORD record={};

        record.sPath        =recordObj.value("Path").toString();
        record.sFullPath    =sRootPath+QDir::separator()+record.sPath;
        record.sSHA1        =recordObj.value("SHA1").toString();

        QString sAction=recordObj.value("Action").toString();

        if(sAction=="copy_file")
        {
            record.bIsFile=true;
        }

        result.listInstallRecords.append(record);
    }

    int nRemoveCount=removeArray.count();

    for(int i=0;i<nRemoveCount;i++)
    {
        QJsonObject recordObj=removeArray.at(i).toObject();
        REMOVE_RECORD record={};

        record.sPath        =recordObj.value("Path").toString();
        record.sFullPath    =sRootPath+QDir::separator()+record.sPath;

        QString sAction=recordObj.value("Action").toString();

        if(sAction=="remove_file")
        {
            record.action=RRA_REMOVEFILE;
        }
        else if(sAction=="remove_directory_if_empty")
        {
            record.action=RRA_REMOVEDIRECTORYIFEMPTY;
        }

        result.listRemoveRecords.append(record);
    }

    return result;
}

Utils::MDATA Utils::getMDataFromJSONFile(QString sFileName, QString sRootPath)
{
    Utils::MDATA result={};

    QByteArray baData=XBinary::readFile(sFileName);

    result=getMDataFromData(baData,sRootPath);

    return result;
}

QList<Utils::MDATA> Utils::getInstalledModules(QString sDataPath, QString sRootPath)
{
    QList<Utils::MDATA> listResult;

    QDir dir(sDataPath+QDir::separator()+"installed");

    QFileInfoList listFiles=dir.entryInfoList(QStringList()<<"*.json",QDir::Files|QDir::NoSymLinks,QDir::Name);

    int nCount=listFiles.count();

    for(int i=0;i<nCount;i++)
    {
        QString sBundleName=listFiles.at(i).filePath();

        QFile file;
        file.setFileName(sBundleName);

        if(file.open(QIODevice::ReadOnly))
        {
            QByteArray baData=file.readAll();

            Utils::MDATA record=getMDataFromData(baData,sRootPath);
            record.sBundleFileName=sBundleName;

            listResult.append(record);

            file.close();
        }
    }

    return listResult;
}

QList<Utils::MDATA> Utils::getModulesFromJSONFile(QString sFileName)
{
    QList<Utils::MDATA> listResult;

    QByteArray baData=XBinary::readFile(sFileName);

    QJsonDocument jsDoc=QJsonDocument::fromJson(baData);

    QJsonObject rootObj=jsDoc.object();

//    QString sDate=rootObj.value("Date").toString();

    QJsonArray arrayModules=rootObj.value("Modules").toArray();

    int nCount=arrayModules.count();

    for(int i=0;i<nCount;i++)
    {
        QJsonObject recordObject=arrayModules.at(i).toObject();

        MDATA mdata={};

        objectToMData(&recordObject,&mdata);

        listResult.append(mdata);
    }

    return listResult;
}

bool Utils::createServerList(QString sListFileName, QList<QString> *pList, QString sWebPrefix, QString sDate)
{
    bool bResult=false;

    QJsonArray arrayModules;

    int nCount=pList->count();

    for(int i=0;i<nCount;i++)
    {
        MDATA mdata=getMDataFromJSONFile(pList->at(i),"");
        mdata.sSrc=sWebPrefix+"/"+mdata.sSrc;

        QJsonObject record;

        mDataToObject(&mdata,&record);

        arrayModules.append(record);
    }

    QJsonObject recordObject;
    recordObject.insert("Date",      sDate);
    recordObject.insert("Modules",   arrayModules);

    QByteArray baResult;

    QJsonDocument doc(recordObject);
    baResult.append(doc.toJson());

    bResult=XBinary::writeToFile(sListFileName,baResult);

    return bResult;
}

void Utils::mDataToObject(Utils::MDATA *pMData, QJsonObject *pObject)
{
    pObject->insert("Name",             QJsonValue::fromVariant(pMData->sName));
    pObject->insert("Version",          QJsonValue::fromVariant(pMData->sVersion));
    pObject->insert("Date",             QJsonValue::fromVariant(pMData->sDate));
    pObject->insert("Author",           QJsonValue::fromVariant(pMData->sAuthor));
    pObject->insert("Bugreport",        QJsonValue::fromVariant(pMData->sBugreport));
    pObject->insert("Info",             QJsonValue::fromVariant(pMData->sInfo));
    pObject->insert("Size",             QJsonValue::fromVariant(pMData->nSize));
    pObject->insert("CompressedSize",   QJsonValue::fromVariant(pMData->nCompressedSize));
    pObject->insert("Is32",             QJsonValue::fromVariant(pMData->bIs32));
    pObject->insert("Is64",             QJsonValue::fromVariant(pMData->bIs64));
    pObject->insert("Src",              QJsonValue::fromVariant(pMData->sSrc));

    if(pMData->sSHA1!="")
    {
        pObject->insert("SHA1",             QJsonValue::fromVariant(pMData->sSHA1));
    }
}

void Utils::objectToMData(QJsonObject *pObject, Utils::MDATA *pMData)
{
    pMData->sName           =pObject->value("Name").toString();
    pMData->sVersion        =pObject->value("Version").toString();
    pMData->sDate           =pObject->value("Date").toString();
    pMData->sAuthor         =pObject->value("Author").toString();
    pMData->sBugreport      =pObject->value("Bugreport").toString();
    pMData->sInfo           =pObject->value("Info").toString();
    pMData->nSize           =pObject->value("Size").toInt();
    pMData->nCompressedSize =pObject->value("CompressedSize").toInt();
    pMData->bIs32           =pObject->value("Is32").toBool();
    pMData->bIs64           =pObject->value("Is64").toBool();
    pMData->sSrc            =pObject->value("Src").toString();
    pMData->sSHA1           =pObject->value("SHA1").toString();
}

QMap<QString, Utils::STATUS> Utils::getModulesStatusMap(XPLUGINMANAGER::OPTIONS *pOptions,QList<Utils::MDATA> *pServerList, QList<Utils::MDATA> *pInstalled)
{
    QMap<QString, Utils::STATUS> mapResult;

    int nCount=pServerList->count();

    for(int i=0;i<nCount;i++)
    {
        STATUS status={};
        status.bInstall=true;
        status.sServerListDate=pServerList->at(i).sDate;
        status.sServerListVersion=pServerList->at(i).sVersion;
        status.webRecord.sName=pServerList->at(i).sName;
        status.webRecord.sFileName=getModuleFileName(pOptions,pServerList->at(i).sName);
        status.webRecord.sLink=pServerList->at(i).sSrc;

        mapResult.insert(pServerList->at(i).sName,status);
    }

    nCount=pInstalled->count();

    for(int i=0;i<nCount;i++)
    {
        QString sName=pInstalled->at(i).sName;
        bool bIsServerList=mapResult.contains(sName);

        STATUS status={};

        if(bIsServerList)
        {
            status=mapResult.value(sName);
        }
        status.webRecord.sName=pInstalled->at(i).sName;
        status.bInstall=false;
        status.bRemove=true;
        status.sInstalledDate=pInstalled->at(i).sDate;
        status.sInstalledVersion=pInstalled->at(i).sVersion;

        if(bIsServerList)
        {
            if((status.sServerListDate>status.sInstalledDate)&&(status.sServerListVersion!=status.sInstalledVersion))
            {
                status.bUpdate=true;
            }
        }

        mapResult.insert(pInstalled->at(i).sName,status);
    }

    return mapResult;
}

Utils::MODULES_DATA Utils::getModulesData(XPLUGINMANAGER::OPTIONS *pOptions)
{
    MODULES_DATA result={};

    result.listServerList=Utils::getModulesFromJSONFile(Utils::getServerListFileName(pOptions));
    result.listInstalled=Utils::getInstalledModules(XBinary::convertPathName(pOptions->sDataPath),XBinary::convertPathName(pOptions->sRootPath));
    result.mapStatus=getModulesStatusMap(pOptions,&result.listServerList,&result.listInstalled);
    result.listUpdates=getUpdates(&result.mapStatus);

    return result;
}

QList<Utils::WEB_RECORD> Utils::getUpdates(QMap<QString,STATUS> *pMapStatus)
{
    QList<Utils::WEB_RECORD> listResult;

    QList<STATUS> listStatuses=pMapStatus->values();

    int nCount=listStatuses.count();

    for(int i=0;i<nCount;i++)
    {
        if(listStatuses.at(i).bUpdate)
        {
            WEB_RECORD record=listStatuses.at(i).webRecord;

            listResult.append(record);
        }
    }

    return listResult;
}

QString Utils::getInstalledJsonFileName(XPLUGINMANAGER::OPTIONS *pOptions, QString sName)
{
    return XBinary::convertPathName(pOptions->sDataPath)+QDir::separator()+"installed"+QDir::separator()+QString("%1.json").arg(sName);
}

QString Utils::getServerListFileName(XPLUGINMANAGER::OPTIONS *pOptions)
{
    return XBinary::convertPathName(pOptions->sDataPath)+QDir::separator()+"list.json";
}

QString Utils::getModuleFileName(XPLUGINMANAGER::OPTIONS *pOptions, QString sName)
{
    return XBinary::convertPathName(pOptions->sDataPath)+QDir::separator()+"modules"+QDir::separator()+QString("%1.x64dbg.zip").arg(sName);
}

Utils::MDATA Utils::getMDataByName(QList<MDATA> *pServerList,QString sName)
{
    Utils::MDATA result={};

    int nCount=pServerList->count();

    for(int i=0;i<nCount;i++)
    {
        if(pServerList->at(i).sName==sName)
        {
            result=pServerList->at(i);

            break;
        }
    }

    return result;
}

QList<QString> Utils::getNamesFromWebRecords(QList<Utils::WEB_RECORD> *pListWebRecords)
{
    QList<QString> listResult;

    int nCount=pListWebRecords->count();

    for(int i=0;i<nCount;i++)
    {
        listResult.append(pListWebRecords->at(i).sName);
    }

    return listResult;
}

Utils::WEB_RECORD Utils::getWebRecordByName(QList<Utils::WEB_RECORD> *pListWebRecords, QString sName)
{
    Utils::WEB_RECORD result={};

    int nCount=pListWebRecords->count();

    for(int i=0;i<nCount;i++)
    {
        if(pListWebRecords->at(i).sName==sName)
        {
            result=pListWebRecords->at(i);

            break;
        }
    }

    return result;
}

void Utils::_getRecords(QString sRootPath, QString sCurrentPath, QList<Utils::RECORD> *pListRecords)
{
    QFileInfo fi(sCurrentPath);

    RECORD record={};

    record.bIsFile=true;
    record.sFullPath=fi.absoluteFilePath();
    record.sPath=record.sFullPath;
    record.bIsFile=fi.isFile();

    record.sPath=record.sPath.remove(0,sRootPath.size()+1);

    if(record.sPath!="")
    {
        pListRecords->append(record);
    }

    if(!record.bIsFile)
    {
        QDir dir(sCurrentPath);
        QFileInfoList eil=dir.entryInfoList();

        int nCount=eil.count();

        for(int i=0;i<nCount;i++)
        {
            QString sFN=eil.at(i).fileName();

            if((sFN!=".")&&(sFN!=".."))
            {
                _getRecords(sRootPath,eil.at(i).absoluteFilePath(),pListRecords);
            }
        }
    }
}
