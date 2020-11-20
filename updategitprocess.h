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
#ifndef UPDATEGITPROCESS_H
#define UPDATEGITPROCESS_H

#include <QObject>
#include <QElapsedTimer>
#include <QThread>
#include "utils.h"
#include "xgithub.h"

class UpdateGitProcess : public QObject
{
    Q_OBJECT

public:
    explicit UpdateGitProcess(QObject *pParent=nullptr);
    void setData(QString sServerListFileName);
    void stop();
    Utils::STATS getCurrentStats();
    void setCredentials(QString sUser, QString sToken);

signals:
    void errorMessage(QString sMessage);
    void infoMessage(QString sMessage);
    void completed(qint64 nElapsedTime);

public slots:
    void process();

private:
    QString sServerListFileName;
    bool bIsStop;
    Utils::STATS currentStats;
    QString sAuthUser;
    QString sAuthToken;
};

#endif // UPDATEGITPROCESS_H
