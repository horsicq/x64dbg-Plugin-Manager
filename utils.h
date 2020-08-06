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
#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include "global.h"
#include "xzip.h"

class Utils : public QObject
{
    Q_OBJECT

public:
    struct WEB_RECORD
    {
        QString sName;
        QString sFileName;
        QString sLink;
    };

    struct STATUS
    {
        bool bInstall;
        bool bUpdate;
        bool bRemove;
        QString sServerListVersion;
        QString sServerListDate;
        QString sInstalledVersion;
        QString sInstalledDate;
        WEB_RECORD webRecord;
    };

    struct STATS
    {
        QString sModule;
        qint64 nTotalModule;
        qint64 nCurrentModule;
        QString sFile;
        qint64 nTotalFile;
        qint64 nCurrentFile;
    };

    struct RECORD
    {
        QString sFullPath;
        QString sPath;
        bool bIsFile;
    };

    struct INSTALL_RECORD
    {
        QString sFullPath;
        QString sPath;
        bool bIsFile;
        QString sSHA1;
    };

    enum RRA
    {
        RRA_UNKNOWN=0,
        RRA_REMOVEFILE,
        RRA_REMOVEDIRECTORYIFEMPTY
    };

    struct REMOVE_RECORD
    {
        QString sFullPath;
        QString sPath;
        RRA action;
    };

    struct MDATA
    {
        QString sName;
        QString sVersion;
        bool bIs32;
        bool bIs64;
        QString sDate;
        QString sAuthor;
        QString sBugreport;
        QString sInfo;
        QString sRoot;
        QString sBundleFileName;
        QList<Utils::INSTALL_RECORD> listInstallRecords;
        QList<Utils::REMOVE_RECORD> listRemoveRecords;
        qint64 nSize;
        qint64 nCompressedSize;
        QString sSrc;
        QString sSHA1;
    };

    struct FILE_RECORD
    {
        QString sFullPath;
        QString sPath;
        QString sSHA1;
//        qint64 nSize;
//        qint64 nCompressedSize;
    };

    struct DIRECTORY_RECORD
    {
        QString sFullPath;
        QString sPath;
    };

    struct MODULES_DATA
    {
        QList<Utils::MDATA> listServerList;
        QList<Utils::MDATA> listInstalled;
        QMap<QString,STATUS> mapStatus;
        QList<WEB_RECORD> listUpdates;
    };

    explicit Utils(QObject *pParent=nullptr);

    static void loadOptions(XPLUGINMANAGER::OPTIONS *pOptions);
    static void saveOptions(XPLUGINMANAGER::OPTIONS *pOptions);

    static QList<RECORD> getRecords(QString sRootPath);
    static bool checkMData(MDATA *pMData,QString *psErrorString);
    static QString createBundleName(MDATA *pMData);
    static bool isPluginValid(QString sFileName);
    static bool isPluginValid(QIODevice *pDevice);

    static QByteArray createPluginInfo(Utils::MDATA *pMData, QList<Utils::FILE_RECORD> *pListFileRecords, QList<Utils::DIRECTORY_RECORD> *pListDirectoryRecords);
    static MDATA getMDataFromZip(QString sFileName,QString sRootPath);
    static MDATA getMDataFromZip(QIODevice *pDevice,QString sRootPath);
    static MDATA getMDataFromData(QByteArray baData, QString sRootPath);
    static MDATA getMDataFromJSONFile(QString sFileName, QString sRootPath);

    static QList<MDATA> getInstalledModules(QString sDataPath,QString sRootPath);
    static QList<MDATA> getModulesFromJSONFile(QString sFileName);

    static bool createServerList(QString sListFileName,QList<QString> *pList,QString sWebPrefix,QString sDate);

    static void mDataToObject(Utils::MDATA *pMData,QJsonObject *pObject);
    static void objectToMData(QJsonObject *pObject,Utils::MDATA *pMData);

    static QMap<QString,STATUS> getModulesStatusMap(XPLUGINMANAGER::OPTIONS *pOptions,QList<MDATA> *pServerList,QList<MDATA> *pInstalled);

    static MODULES_DATA getModulesData(XPLUGINMANAGER::OPTIONS *pOptions);
    static QList<Utils::WEB_RECORD> getUpdates(QMap<QString, STATUS> *pMapStatus);

    static QString getInstalledJsonFileName(XPLUGINMANAGER::OPTIONS *pOptions,QString sName);
    static QString getServerListFileName(XPLUGINMANAGER::OPTIONS *pOptions);
    static QString getModuleFileName(XPLUGINMANAGER::OPTIONS *pOptions,QString sName);

    static Utils::MDATA getMDataByName(QList<MDATA> *pServerList,QString sName);
    static QList<QString> getNamesFromWebRecords(QList<WEB_RECORD> *pListWebRecords);
    static Utils::WEB_RECORD getWebRecordByName(QList<WEB_RECORD> *pListWebRecords,QString sName);

private:
    static void _getRecords(QString sRootPath,QString sCurrentPath,QList<RECORD> *pListRecords);
};

#endif // UTILS_H
