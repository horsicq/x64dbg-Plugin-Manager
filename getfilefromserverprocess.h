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
#ifndef GETFILEFROMSERVERPROCESS_H
#define GETFILEFROMSERVERPROCESS_H

#include <QObject>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "utils.h"

class GetFileFromServerProcess : public QObject
{
    Q_OBJECT
public:
    explicit GetFileFromServerProcess(QObject *pParent=nullptr);
    void setData(QList<Utils::WEB_RECORD> listWebRecords);
    void stop();
    Utils::STATS getCurrentStats();

signals:
    void errorMessage(QString sMessage);
    void infoMessage(QString sMessage);
    void completed(qint64 nElapsedTime);

public slots:
    void process();
    void _downloadProgress(qint64 bytesReceived,qint64 bytesTotal);

private:
    bool bIsStop;
    QList<Utils::WEB_RECORD> listWebRecords;
    QNetworkReply *pReply;
    QNetworkReply *replyRed;
    Utils::STATS currentStats;
};

#endif // GETFILEFROMSERVERPROCESS_H
