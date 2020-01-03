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
#include "removemoduleprocess.h"

RemoveModuleProcess::RemoveModuleProcess(QObject *parent) : QObject(parent)
{
    bIsStop=false;
    currentStats={};
}

void RemoveModuleProcess::setData(XPLUGINMANAGER::OPTIONS *pOptions, QList<QString> listModuleNames)
{
    this->pOptions=pOptions;
    this->listModuleNames=listModuleNames;
}

void RemoveModuleProcess::stop()
{
    bIsStop=true;
}

Utils::STATS RemoveModuleProcess::getCurrentStats()
{
    return currentStats;
}

void RemoveModuleProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    bIsStop=false;

    currentStats.nTotalModule=listModuleNames.count();

    for(qint32 i=0;(i<currentStats.nTotalModule)&&(!bIsStop);i++)
    {
        currentStats.sModule=QString("%1: %2").arg(tr("Remove module")).arg(listModuleNames.at(i));

        emit infoMessage(currentStats.sModule);

        QString sFileName=Utils::getInstalledJsonFileName(pOptions,listModuleNames.at(i));

        if(XBinary::isFileExists(sFileName))
        {
            Utils::MDATA mdata=Utils::getMDataFromJSONFile(sFileName,XBinary::convertPathName(pOptions->sRootPath));

            currentStats.nTotalFile=mdata.listRemoveRecords.count();

            for(qint32 j=0;(j<currentStats.nTotalFile)&&(!bIsStop);j++)
            {
                Utils::REMOVE_RECORD record=mdata.listRemoveRecords.at(j);

                currentStats.sFile=QString("%1: %2").arg(tr("Remove file")).arg(record.sPath);

                emit infoMessage(currentStats.sFile);

                if(record.action==Utils::RRA_REMOVEFILE)
                {
                    XBinary::removeFile(record.sFullPath);

                    if(XBinary::isFileExists(record.sFullPath))
                    {
                        emit errorMessage(tr("%1: %2").arg(tr("Cannot remove file")).arg(record.sFullPath));
                        bIsStop=true;
                    }
                }
                else if(record.action==Utils::RRA_REMOVEDIRECTORYIFEMPTY)
                {
                    if(XBinary::isDirectoryEmpty(record.sFullPath))
                    {
                        XBinary::removeDirectory(record.sFullPath);

                        if(XBinary::isDirectoryExists(record.sFullPath))
                        {
                            emit errorMessage(tr("%1: %2").arg(tr("Cannot remove directory")).arg(record.sFullPath));
                            bIsStop=true;
                        }
                    }
                }

                currentStats.nCurrentFile=j+1;
            }

            if(!bIsStop)
            {
                XBinary::removeFile(sFileName);
            }
            else
            {
                emit errorMessage(tr("Please, close all applications and try again."));
            }
        }

        currentStats.nCurrentModule=i+1;
    }

    emit completed(elapsedTimer.elapsed());
}
