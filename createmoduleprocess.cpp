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
#include "createmoduleprocess.h"

CreateModuleProcess::CreateModuleProcess(QObject *parent) : QObject(parent)
{

}

void CreateModuleProcess::setData(Utils::MDATA *pMData)
{
    this->pMData=pMData;
}

void CreateModuleProcess::stop()
{
    // TODO
}

void CreateModuleProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    QList<Utils::FILE_RECORD> listFileRecords;
    QList<Utils::DIRECTORY_RECORD> listDirectoryRecords;

    int nCount=pMData->listRecords.count();

    qint64 nTotalSize=0;

    QString sBundleFileName=pMData->sBundlePath+QDir::separator()+pMData->sBundleName+".zip";

    bool bSuccess=true;

    if(XBinary::isFileExists(sBundleFileName))
    {
        bSuccess=XBinary::removeFile(sBundleFileName);

        if(!bSuccess)
        {
            emit errorMessage(tr("Cannot remove: %1").arg(sBundleFileName));
        }
    }

    if(bSuccess)
    {
        QFile fileResult;

        fileResult.setFileName(sBundleFileName);

        if(fileResult.open(QIODevice::ReadWrite))
        {
            for(int i=0;i<nCount;i++)
            {
                if(pMData->listRecords.at(i).bIsFile)
                {
                    QFile file;

                    file.setFileName(pMData->listRecords.at(i).sFullPath);

                    if(file.open(QIODevice::ReadOnly))
                    {
                        Utils::FILE_RECORD fileRecord={};

                        fileRecord.sFullPath=pMData->listRecords.at(i).sFullPath;
                        fileRecord.sPath=pMData->listRecords.at(i).sPath;
                        fileRecord.nSize=file.size();
                        fileRecord.sSHA1=XBinary::getHash(XBinary::HASH_SHA1,&file);

                        listFileRecords.append(fileRecord);

                        XArchive::compress(XArchive::COMPRESS_METHOD_DEFLATE,&file,&fileResult);
                        // TODO
                        file.close();

                        nTotalSize+=fileRecord.nSize;
                    }
                }
                else
                {
                    Utils::DIRECTORY_RECORD directoryRecord={};

                    directoryRecord.sFullPath=pMData->listRecords.at(i).sFullPath;
                    directoryRecord.sPath=pMData->listRecords.at(i).sPath;

                    listDirectoryRecords.append(directoryRecord);
                }
            }

            fileResult.close();
        }
    }

    // TODO
    emit completed(elapsedTimer.elapsed());
}
