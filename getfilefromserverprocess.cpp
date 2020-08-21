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
#include "getfilefromserverprocess.h"

GetFileFromServerProcess::GetFileFromServerProcess(QObject *pParent) : QObject(pParent)
{
    pReply=0;
    replyRed=0;
    currentStats={};
}

void GetFileFromServerProcess::setData(QList<Utils::WEB_RECORD> listWebRecords)
{
    this->listWebRecords=listWebRecords;
}

void GetFileFromServerProcess::stop()
{
    bIsStop=true;

    if(pReply)
    {
        pReply->abort();
    }

    if(replyRed)
    {
        replyRed->abort();
    }
}

Utils::STATS GetFileFromServerProcess::getCurrentStats()
{
    return currentStats;
}

void GetFileFromServerProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    bIsStop=false;

    currentStats.nTotalModule=listWebRecords.count();

    for(qint32 i=0;(i<currentStats.nTotalModule)&&(!bIsStop);i++)
    {
        currentStats.sModule=listWebRecords.at(i).sLink;
        emit infoMessage(currentStats.sModule);

        QNetworkAccessManager nam;
        QNetworkRequest *pRequest=new QNetworkRequest(QUrl(listWebRecords.at(i).sLink));
        pReply=nam.get(*pRequest);
        QEventLoop loop;
        QObject::connect(pReply,SIGNAL(finished()),&loop,SLOT(quit()));
        QObject::connect(pReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(_downloadProgress(qint64,qint64)));
        loop.exec();

        if(pReply->error()==QNetworkReply::NoError)
        {
            if(pReply->bytesAvailable())
            {
                bool bSuccess=false;
                QByteArray baData;

                QString sRedirectUrl=pReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

                if(sRedirectUrl!="") // Github redirect
                {
                    QNetworkAccessManager namRed;
                    QNetworkRequest *pRequestRed=new QNetworkRequest(QUrl(sRedirectUrl));
                    replyRed=namRed.get(*pRequestRed);
                    QEventLoop loopRed;
                    QObject::connect(replyRed,SIGNAL(finished()),&loopRed,SLOT(quit()));
                    QObject::connect(replyRed,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(_downloadProgress(qint64,qint64)));
                    loopRed.exec();

                    if(replyRed->bytesAvailable())
                    {
                        baData=replyRed->readAll();
                        bSuccess=true;
                    }
                }
                else
                {
                    baData=pReply->readAll();
                    bSuccess=true;
                }

                if(bSuccess)
                {
                    if(XBinary::isFileExists(listWebRecords.at(i).sFileName))
                    {
                        if(!XBinary::removeFile(listWebRecords.at(i).sFileName))
                        {
                            emit errorMessage(QString("%1: %2").arg(tr("Cannot remove file")).arg(listWebRecords.at(i).sFileName));
                            bIsStop=true;
                        }
                    }

                    if(XBinary::writeToFile(listWebRecords.at(i).sFileName,baData))
                    {
                        currentStats.sFile=QString("%1: %2").arg(tr("Write data to file")).arg(listWebRecords.at(i).sFileName);
                        emit infoMessage(currentStats.sFile);
                    }
                    else
                    {
                        emit errorMessage(QString("%1: %2").arg(tr("Cannot write data to file")).arg(listWebRecords.at(i).sFileName));
                    }
                }
            }
        }
        else
        {
            emit errorMessage(pReply->errorString());
            bIsStop=true;
        }

        pReply->deleteLater();
        delete pRequest;

        pReply=0;

        currentStats.nCurrentModule=i;
    }

    bIsStop=false;

    emit completed(elapsedTimer.elapsed());
}

void GetFileFromServerProcess::_downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    currentStats.nTotalBytes=bytesTotal;
    currentStats.nCurrentBytes=bytesReceived;
}
