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
#include "installmoduleprocess.h"

InstallModuleProcess::InstallModuleProcess(QObject *parent) : QObject(parent)
{
    bIsStop=false;
    currentStats={};
}

void InstallModuleProcess::setData(Utils::MDATA *pMData, QIODevice *pDevice, QString sDataPath)
{
    this->pMData=pMData;
    this->pDevice=pDevice;
    this->sDataPath=sDataPath;
}

void InstallModuleProcess::stop()
{
    bIsStop=true;
}

Utils::STATS InstallModuleProcess::getCurrentStats()
{
    return currentStats;
}

void InstallModuleProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    bIsStop=false;

    XZip zip(pDevice);

    QList<XArchive::RECORD> listZipRecords=zip.getRecords();

    int nCount=pMData->listRecords.count();

    for(int i=0;(i<nCount)&&(!bIsStop);i++)
    {
        Utils::RECORD record=pMData->listRecords.at(i);

        if(record.bIsFile)
        {
            XArchive::RECORD archiveRecord=XArchive::getArchiveRecord("files/"+record.sPath,&listZipRecords);
            zip.decompressToFile(&archiveRecord,record.sFullPath);

            if(XBinary::getHash(XBinary::HASH_SHA1,record.sFullPath)!=record.sSHA1)
            {
                qDebug("INVALID HASH"); // TODO
            }
        }
        else
        {
            XBinary::createDirectory(record.sFullPath);
        }
    }

    // TODO save info

    QString sInfoFileName=XBinary::convertPathName(sDataPath)+QDir::separator()+pMData->sName+".json";

    XArchive::RECORD archiveRecord=XArchive::getArchiveRecord("plugin_info.json",&listZipRecords);
    zip.decompressToFile(&archiveRecord,sInfoFileName);

    emit completed(elapsedTimer.elapsed());
}
