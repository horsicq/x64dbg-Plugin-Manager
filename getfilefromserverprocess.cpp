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
#include "getfilefromserverprocess.h"

GetFileFromServerProcess::GetFileFromServerProcess(QObject *parent) : QObject(parent)
{
    reply=0;
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

    if(reply)
    {
        reply->abort();
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

        QNetworkAccessManager nam;
        QNetworkRequest *pRequest=new QNetworkRequest(QUrl(listWebRecords.at(i).sLink));
        reply=nam.get(*pRequest);
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if(reply->bytesAvailable())
        {
            bool bSuccess=false;
            QByteArray baData;

            QString sRedirectUrl=reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

            if(sRedirectUrl!="") // Github redirect
            {
                QNetworkAccessManager namRed;
                QNetworkRequest *pRequestRed=new QNetworkRequest(QUrl(sRedirectUrl));
                replyRed=namRed.get(*pRequestRed);
                QEventLoop loopRed;
                QObject::connect(replyRed, SIGNAL(finished()), &loopRed, SLOT(quit()));
                loopRed.exec();

                if(replyRed->bytesAvailable())
                {
                    baData=replyRed->readAll();
                    bSuccess=true;
                }
            }
            else
            {
                baData=reply->readAll();
                bSuccess=true;
            }

            if(bSuccess)
            {
                if(XBinary::isFileExists(listWebRecords.at(i).sFileName))
                {
                    XBinary::removeFile(listWebRecords.at(i).sFileName);
                    // TODO handle errors
                }

                XBinary::writeToFile(listWebRecords.at(i).sFileName,baData); // TODO handle errors
            }
        }

        reply->deleteLater();
        delete pRequest;

        reply=0;
    }

    bIsStop=false;

    emit completed(elapsedTimer.elapsed());
}
