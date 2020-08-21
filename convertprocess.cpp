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
#include "convertprocess.h"

ConvertProcess::ConvertProcess(QObject *pParent) : QObject(pParent)
{
    bIsStop=false;
    currentStats={};
}

void ConvertProcess::setData(Utils::MDATA *pMData, QString sDataPath)
{
    this->pMData=pMData;
    this->sDataPath=XBinary::convertPathName(sDataPath);
}

void ConvertProcess::stop()
{
    bIsStop=true;
}

Utils::STATS ConvertProcess::getCurrentStats()
{
    return currentStats;
}

void ConvertProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    bIsStop=false;

    QString sGithubZipModulePath=Utils::getGithubZipModulePath(sDataPath,pMData->sName);

    currentStats.nTotalModule=pMData->listConvertRecords.count();

    QFile file;
    file.setFileName(Utils::getGithubZipDownloadFileName(sDataPath,pMData->sName));

    if(file.open(QIODevice::ReadOnly))
    {
        XZip zip(&file);

        QList<XArchive::RECORD> listZipRecords=zip.getRecords();

        for(int i=0;(i<currentStats.nTotalModule)&&(!bIsStop);i++)
        {
            Utils::HANDLE_RECORD handleRecord=pMData->listConvertRecords.at(i);

            // TODO errors
            if(handleRecord.action==Utils::ACTION_MAKEDIRECTORY)
            {
                XBinary::createDirectory(sGithubZipModulePath+QDir::separator()+handleRecord.sPath);
            }
            else if(handleRecord.action==Utils::ACTION_UNPACKFILE)
            {
                zip.decompressToFile(&listZipRecords,handleRecord.sSrc,sGithubZipModulePath+QDir::separator()+handleRecord.sPath);
            }
            else if(handleRecord.action==Utils::ACTION_UNPACKDIRECTORY)
            {
                zip.decompressToPath(&listZipRecords,handleRecord.sSrc,sGithubZipModulePath+QDir::separator()+handleRecord.sPath);
            }

            currentStats.nCurrentModule=i;
        }

        file.close();
    }

    emit completed(elapsedTimer.elapsed());
}
