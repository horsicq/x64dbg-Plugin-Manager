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
#include "utils.h"

Utils::Utils(QObject *parent) : QObject(parent)
{

}

QList<Utils::RECORD> Utils::getRecords(QString sRootPath)
{
    QList<Utils::RECORD> listResult;

    QFileInfo fi(sRootPath);

    sRootPath=fi.absoluteDir().path();

    _getRecords(sRootPath,sRootPath,&listResult);

    return listResult;
}

void Utils::_getRecords(QString sRootPath, QString sCurrentPath, QList<Utils::RECORD> *pListRecords)
{
    QFileInfo fi(sCurrentPath);

    RECORD record={};

    record.bIsFile=true;
    record.sFullPath=fi.absoluteFilePath();
    record.sPath=record.sFullPath; // TODO
    record.bIsFile=fi.isFile();

    pListRecords->append(record);

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
