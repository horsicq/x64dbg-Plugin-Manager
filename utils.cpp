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

Utils::Utils(QObject *pParent) : QObject(pParent)
{

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
            record.insert("Action",     actionIdToString(ACTION_MAKEDIRECTORY));

            installArray.append(record);
        }

        for(int i=0;i<nFilesCount;i++)
        {
            QJsonObject record;

            record.insert("Path",       pListFileRecords->at(i).sPath);
            record.insert("Action",     actionIdToString(ACTION_COPYFILE));
            record.insert("SHA1",       pListFileRecords->at(i).sSHA1);

            installArray.append(record);
        }

        recordObject.insert("Install",  installArray);

        QJsonArray removeArray;

        for(int i=0;i<nFilesCount;i++)
        {
            QJsonObject record;

            record.insert("Path",       pListFileRecords->at(i).sPath);
            record.insert("Action",     actionIdToString(ACTION_REMOVEFILE));

            removeArray.append(record);
        }

        for(int i=nDirectoriesCount-1;i>=0;i--)
        {
            QJsonObject record;

            record.insert("Path",       pListDirectoryRecords->at(i).sPath);
            record.insert("Action",     actionIdToString(ACTION_REMOVEDIRECTORYIFEMPTY));

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

        result=getMDataFromData(baData);
    }

    return result;
}

Utils::MDATA Utils::getMDataFromData(QByteArray baData)
{
    Utils::MDATA result={};

    QJsonDocument jsDoc=QJsonDocument::fromJson(baData);

    QJsonObject rootObj=jsDoc.object();

    objectToMData(&rootObj,&result);

    return result;
}

Utils::MDATA Utils::getMDataFromJSONFile(QString sFileName)
{
    Utils::MDATA result={};

    QByteArray baData=XBinary::readFile(sFileName);

    result=getMDataFromData(baData);

    return result;
}

QList<Utils::MDATA> Utils::getInstalledModules(QString sDataPath, QString sRootPath)
{
    QList<Utils::MDATA> listResult;

    sDataPath=XBinary::convertPathName(sDataPath);
    sRootPath=XBinary::convertPathName(sRootPath);

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

            Utils::MDATA record=getMDataFromData(baData);
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

QDate Utils::getDateFromJSONFile(QString sFileName)
{
    QDate dtResult;

    QByteArray baData=XBinary::readFile(sFileName);

    QJsonDocument jsDoc=QJsonDocument::fromJson(baData);

    QJsonObject rootObj=jsDoc.object();

    dtResult=QDate::fromString(rootObj.value("Date").toString(),"yyyy-MM-dd");

    return dtResult;
}

bool Utils::createServerList(QString sListFileName, QList<QString> *pList, QString sWebPrefix, QString sDate)
{
    bool bResult=false;

    QJsonArray arrayModules;

    int nCount=pList->count();

    for(int i=0;i<nCount;i++)
    {
        MDATA mdata=getMDataFromJSONFile(pList->at(i));
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
        pObject->insert("SHA1",         QJsonValue::fromVariant(pMData->sSHA1));
    }

    if(pMData->sGithub!="")
    {
        pObject->insert("Github",       QJsonValue::fromVariant(pMData->sGithub));
    }

    // TODO optimize
    int nInstallCount=pMData->listInstallRecords.count();
    int nRemoveCount=pMData->listRemoveRecords.count();
    int nConvertCount=pMData->listConvertRecords.count();
    int nDownloadCount=pMData->listDownloads.count();

    if(nInstallCount)
    {
        QJsonArray jsArray;

        for(int i=0;i<nInstallCount;i++)
        {
            HANDLE_RECORD record=pMData->listInstallRecords.at(i);
            QJsonObject recordObj;

            handleRecordToObject(&record,&recordObj);

            jsArray.append(recordObj);
        }

        pObject->insert("Install",jsArray);
    }

    if(nRemoveCount)
    {
        QJsonArray jsArray;

        for(int i=0;i<nRemoveCount;i++)
        {
            HANDLE_RECORD record=pMData->listRemoveRecords.at(i);
            QJsonObject recordObj;

            handleRecordToObject(&record,&recordObj);

            jsArray.append(recordObj);
        }

        pObject->insert("Remove",jsArray);
    }

    if(nConvertCount)
    {
        QJsonArray jsArray;

        for(int i=0;i<nConvertCount;i++)
        {
            HANDLE_RECORD record=pMData->listConvertRecords.at(i);
            QJsonObject recordObj;

            handleRecordToObject(&record,&recordObj);

            jsArray.append(recordObj);
        }

        pObject->insert("Convert",jsArray);
    }

    if(nDownloadCount)
    {
        QJsonArray jsArray;

        for(int i=0;i<nDownloadCount;i++)
        {
            QJsonObject recordObj;

            recordObj.insert("Src",QJsonValue::fromVariant(pMData->listDownloads.at(i)));

            jsArray.append(recordObj);
        }

        pObject->insert("Download",jsArray);
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
    pMData->sGithub         =pObject->value("Github").toString();

    // TODO optimize
    QJsonArray installArray=pObject->value("Install").toArray();
    QJsonArray removeArray=pObject->value("Remove").toArray();
    QJsonArray convertArray=pObject->value("Convert").toArray();
    QJsonArray downloadArray=pObject->value("Download").toArray();

    int nInstallCount=installArray.count();

    for(int i=0;i<nInstallCount;i++)
    {
        QJsonObject recordObj=installArray.at(i).toObject();
        HANDLE_RECORD record={};

        objectToHandleRecord(&recordObj,&record);

        pMData->listInstallRecords.append(record);
    }

    int nRemoveCount=removeArray.count();

    for(int i=0;i<nRemoveCount;i++)
    {
        QJsonObject recordObj=removeArray.at(i).toObject();
        HANDLE_RECORD record={};

        objectToHandleRecord(&recordObj,&record);

        pMData->listRemoveRecords.append(record);
    }

    int nConvertCount=convertArray.count();

    for(int i=0;i<nConvertCount;i++)
    {
        QJsonObject recordObj=convertArray.at(i).toObject();
        HANDLE_RECORD record={};

        objectToHandleRecord(&recordObj,&record);

        pMData->listConvertRecords.append(record);
    }

    int nDownloadCount=downloadArray.count();

    for(int i=0;i<nDownloadCount;i++)
    {
        QJsonObject recordObj=downloadArray.at(i).toObject();

        QString sRecord=recordObj.value("Src").toString();

        pMData->listDownloads.append(sRecord);
    }
}

void Utils::handleRecordToObject(Utils::HANDLE_RECORD *pHandleRecord, QJsonObject *pObject)
{
    pObject->insert("Path",             QJsonValue::fromVariant(pHandleRecord->sPath));
    pObject->insert("Action",           QJsonValue::fromVariant(actionIdToString(pHandleRecord->action)));

    if(pHandleRecord->sSHA1!="")
    {
        pObject->insert("SHA1",         QJsonValue::fromVariant(pHandleRecord->sSHA1));
    }

    if(pHandleRecord->sSrc!="")
    {
        pObject->insert("Src",          QJsonValue::fromVariant(pHandleRecord->sSrc));
    }

    if(pHandleRecord->sPattern!="")
    {
        pObject->insert("Pattern",      QJsonValue::fromVariant(pHandleRecord->sPattern));
    }
}

void Utils::objectToHandleRecord(QJsonObject *pObject, Utils::HANDLE_RECORD *pHandleRecord)
{
    pHandleRecord->sSrc         =pObject->value("Src").toString();
    pHandleRecord->sPath        =pObject->value("Path").toString();
    pHandleRecord->sSHA1        =pObject->value("SHA1").toString();
    pHandleRecord->sPattern     =pObject->value("Pattern").toString();
    pHandleRecord->action       =stringToActionId(pObject->value("Action").toString());
}

QMap<QString, Utils::STATUS> Utils::getModulesStatusMap(QString sDataPath, QList<Utils::MDATA> *pServerList, QList<Utils::MDATA> *pInstalled)
{
    QMap<QString, Utils::STATUS> mapResult;

    int nCount=pServerList->count();

    for(int i=0;i<nCount;i++)
    {
        STATUS status={};

        if(pServerList->at(i).sDate!="")
        {
            status.bInstall=true;
        }

        status.sServerListDate=pServerList->at(i).sDate;
        status.sServerListVersion=pServerList->at(i).sVersion;
        status.webRecord.sName=pServerList->at(i).sName;
        status.webRecord.sFileName=getModuleFileName(sDataPath,pServerList->at(i).sName);
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
            if((status.sServerListDate>status.sInstalledDate)&&(status.sServerListVersion!=status.sInstalledVersion)&&(status.sServerListDate!=""))
            {
                status.bUpdate=true;
            }
        }

        mapResult.insert(pInstalled->at(i).sName,status);
    }

    return mapResult;
}

Utils::MODULES_DATA Utils::getModulesData(QString sDataPath)
{
    MODULES_DATA result={};

    result.listServerList=Utils::getModulesFromJSONFile(Utils::getServerListFileName(sDataPath));
    result.listInstalled=Utils::getInstalledModules(sDataPath,sDataPath);
    result.mapStatus=getModulesStatusMap(sDataPath,&result.listServerList,&result.listInstalled);
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

QString Utils::getInstalledJsonFileName(QString sDataPath, QString sName)
{
    return XBinary::convertPathName(sDataPath)+QDir::separator()+"installed"+QDir::separator()+QString("%1.json").arg(sName);
}

QString Utils::getServerListFileName(QString sDataPath)
{
    return XBinary::convertPathName(sDataPath)+QDir::separator()+"list.json";
}

QString Utils::getServerLastestListFileName(QString sDataPath)
{
    return XBinary::convertPathName(sDataPath)+QDir::separator()+"list.lastest.json";
}

QString Utils::getModuleFileName(QString sDataPath, QString sName)
{
    return XBinary::convertPathName(sDataPath)+QDir::separator()+"modules"+QDir::separator()+QString("%1.x64dbg.zip").arg(sName);
}

QString Utils::getConvertPath(QString sDataPath, QString sName)
{
    return XBinary::convertPathName(sDataPath)+QDir::separator()+"modules"+QDir::separator()+QString("%1").arg(sName);
}

QString Utils::getConvertDownloadFileName(QString sDataPath, QString sName, QString sPattern)
{
    QString sResult;

    QString sConvertPath=getConvertPath(sDataPath,sName);

    QDir dir(sConvertPath);
    QList<QFileInfo> listFiles=dir.entryInfoList(QDir::Files);

    int nCount=listFiles.count();

    for(int i=0;i<nCount;i++)
    {
        QString sName=listFiles.at(i).fileName();
        if(sName.contains(sPattern))
        {
            sResult=listFiles.at(i).absoluteFilePath();

            break;
        }
    }

    return sResult;
}

QString Utils::getConvertModulePath(QString sDataPath, QString sName)
{
    return XBinary::convertPathName(sDataPath)+QDir::separator()+"modules"+QDir::separator()+QString("%1").arg(sName)+QDir::separator()+"module";
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

bool Utils::isGithubPresent(QString sServerListFileName)
{
    bool bResult=false;

    QList<MDATA> listMData=getModulesFromJSONFile(sServerListFileName);

    int nCount=listMData.count();

    for(int i=0;i<nCount;i++)
    {
        if(listMData.at(i).sGithub!="")
        {
            bResult=true;

            break;
        }
    }

    return bResult;
}

bool Utils::updateJsonFile(QString sFileName, QList<MDATA> listMData)
{
    bool bResult=false;

    QByteArray baData=XBinary::readFile(sFileName);

    QJsonDocument jsDoc=QJsonDocument::fromJson(baData);

    QJsonObject rootObj=jsDoc.object();
    QJsonObject _rootObj;

    _rootObj.insert("Date",rootObj.value("Date"));

    QJsonArray arrayModules=rootObj.value("Modules").toArray();
    QJsonArray _arrayModules;

    int nCount=arrayModules.count();
    int _nCount=listMData.count();

    for(int i=0;i<nCount;i++)
    {
        QJsonObject recordObject=arrayModules.at(i).toObject();

        MDATA mdata={};

        objectToMData(&recordObject,&mdata);

        for(int j=0;j<_nCount;j++)
        {
            if((mdata.sName==listMData.at(j).sName)&&(listMData.at(j).sDate!=""))
            {
                mdata=listMData.at(j);
                break;
            }
        }

        QJsonObject _recordObject;

        mDataToObject(&mdata,&_recordObject);

        _arrayModules.append(_recordObject);
    }

    _rootObj.insert("Modules",_arrayModules);

    QJsonDocument jsResult(_rootObj);

    bResult=XBinary::writeToFile(sFileName,jsResult.toJson(QJsonDocument::Indented));

    return bResult;
}

bool Utils::updateServerList(QString sOldFileName, QString sNewFileName)
{
    bool bResult=false;

    if(!XBinary::isFileExists(sOldFileName))
    {
        bResult=XBinary::copyFile(sNewFileName,sOldFileName);
    }
    else
    {
        if(Utils::getDateFromJSONFile(sNewFileName).toJulianDay()>Utils::getDateFromJSONFile(sOldFileName).toJulianDay())
        {
            bResult=XBinary::copyFile(sNewFileName,sOldFileName);
        }
        else
        {
            QList<Utils::MDATA> listMData=Utils::getModulesFromJSONFile(sNewFileName);

            bResult=updateJsonFile(sOldFileName,listMData);
        }
    }

    return bResult;
}

QString Utils::actionIdToString(Utils::ACTION action)
{
    QString sResult="unknown";

    switch(action)
    {
        case ACTION_UNKNOWN:                    sResult=QString("unknown");                     break;
        case ACTION_COPYFILE:                   sResult=QString("copy_file");                   break;
        case ACTION_REMOVEFILE:                 sResult=QString("remove_file");                 break;
        case ACTION_REMOVEDIRECTORYIFEMPTY:     sResult=QString("remove_directory_if_empty");   break;
        case ACTION_MAKEDIRECTORY:              sResult=QString("make_directory");              break;
        case ACTION_UNPACKDIRECTORY:            sResult=QString("unpack_directory");            break;
        case ACTION_UNPACKFILE:                 sResult=QString("unpack_file");                 break;
    }

    return sResult;
}

Utils::ACTION Utils::stringToActionId(QString sAction)
{
    Utils::ACTION result=ACTION_UNKNOWN;

    if      (sAction=="unknown")                    result=ACTION_UNKNOWN;
    else if (sAction=="copy_file")                  result=ACTION_COPYFILE;
    else if (sAction=="remove_file")                result=ACTION_REMOVEFILE;
    else if (sAction=="remove_directory_if_empty")  result=ACTION_REMOVEDIRECTORYIFEMPTY;
    else if (sAction=="make_directory")             result=ACTION_MAKEDIRECTORY;
    else if (sAction=="unpack_directory")           result=ACTION_UNPACKDIRECTORY;
    else if (sAction=="unpack_file")                result=ACTION_UNPACKFILE;

    return result;
}

bool Utils::checkPattern(QString sString, Utils::MDATA *pMData)
{
    bool bResult=false;

    int nCount=pMData->listConvertRecords.count();

    for(int i=0;i<nCount;i++)
    {
        if(sString.contains(pMData->listConvertRecords.at(i).sPattern))
        {
            bResult=true;

            break;
        }
    }

    return bResult;
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
