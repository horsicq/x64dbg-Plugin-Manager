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
#include "installmoduleprocess.h"

InstallModuleProcess::InstallModuleProcess(QObject *parent) : QObject(parent)
{
    bIsStop=false;
    currentStats={};
}

void InstallModuleProcess::setData(XPLUGINMANAGER::OPTIONS *pOptions, QList<QString> listModuleFileNames)
{
    this->pOptions=pOptions;
    this->listModuleFileNames=listModuleFileNames;
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

    currentStats.nTotalModule=listModuleFileNames.count();

    for(qint32 i=0;(i<currentStats.nTotalModule)&&(!bIsStop);i++)
    {
        currentStats.sModule=QString("%1: %2").arg(tr("Install module")).arg(QFileInfo(listModuleFileNames.at(i)).baseName());
        emit infoMessage(currentStats.sModule);

        QFile file;
        file.setFileName(listModuleFileNames.at(i));

        if(file.open(QIODevice::ReadOnly))
        {
            Utils::MDATA mdata=Utils::getMDataFromZip(&file,XBinary::convertPathName(pOptions->sRootPath));

            XZip zip(&file);

            QList<XArchive::RECORD> listZipRecords=zip.getRecords();

            currentStats.nTotalFile=mdata.listInstallRecords.count();

            for(qint32 j=0;(j<currentStats.nTotalFile)&&(!bIsStop);j++)
            {
                Utils::INSTALL_RECORD record=mdata.listInstallRecords.at(j);

                currentStats.sFile=QString("%1: %2").arg(tr("Install file")).arg(record.sFullPath);
                emit infoMessage(currentStats.sFile);

                if(record.bIsFile)
                {
                    if(XBinary::isFileExists(record.sFullPath))
                    {
                        if(!XBinary::removeFile(record.sFullPath))
                        {
                            emit errorMessage(QString("%1: %2").arg(tr("Cannot remove file")).arg(record.sFullPath));
                            bIsStop=true;
                        }
                    }

                    XArchive::RECORD archiveRecord=XArchive::getArchiveRecord("files/"+record.sPath,&listZipRecords);
                    zip.decompressToFile(&archiveRecord,record.sFullPath);

                    if(!XBinary::isFileHashValid(XBinary::HASH_SHA1,record.sFullPath,record.sSHA1))
                    {
                        emit errorMessage(QString("%1: %2").arg(tr("Invalid file HASH")).arg(record.sFullPath));
                        bIsStop=true;
                    }
                }
                else
                {
                    XBinary::createDirectory(record.sFullPath);
                }

                currentStats.nCurrentFile=j+1;
            }

            QString sInfoFileName=Utils::getInstalledJsonFileName(pOptions,mdata.sName);

            if(XBinary::isFileExists(sInfoFileName))
            {
                if(!XBinary::removeFile(sInfoFileName))
                {
                    emit errorMessage(QString("%1: %2").arg(tr("Cannot remove file")).arg(sInfoFileName));
                    bIsStop=true;
                }
            }

            XArchive::RECORD archiveRecord=XArchive::getArchiveRecord("plugin_info.json",&listZipRecords);
            zip.decompressToFile(&archiveRecord,sInfoFileName);

            file.close();
        }

        currentStats.nCurrentModule=i+1;
    }

    emit completed(elapsedTimer.elapsed());
}
