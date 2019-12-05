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
#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "global.h"
#include "xzip.h"

class Utils : public QObject
{
    Q_OBJECT

public:
    struct STATS
    {
        qint32 nTotal;
        qint32 nCurrent;
        QString sStatus;
    };

    struct RECORD
    {
        QString sFullPath;
        QString sPath;
        bool bIsFile;
    };

    struct MDATA
    {
        QString sName;
        QString sVersion;
        QString sDate;
        QString sInfo;
        QString sRoot;
        QString sBundleName;
        QString sBundlePath;
        QList<Utils::RECORD> listRecords;
        qint64 nSize;
        qint64 nCompressedSize;
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

    explicit Utils(QObject *parent=nullptr);

    static QList<RECORD> getRecords(QString sRootPath);
    static bool checkMData(MDATA *pMData,QString *psErrorString);
    static QString createBundleName(MDATA *pMData);
    static bool isPluginValid(QString sFileName);
    static bool isPluginValid(QIODevice *pDevice);

    static QByteArray createPluginInfo(Utils::MDATA *pMData,QList<Utils::FILE_RECORD> *pListFileRecords,QList<Utils::DIRECTORY_RECORD> *pListDirectoryRecords);
    static MDATA getMDataFromJSON(QIODevice *pDevice,QString sRootPath);

private:
    static void _getRecords(QString sRootPath,QString sCurrentPath,QList<RECORD> *pListRecords);
};

#endif // UTILS_H
