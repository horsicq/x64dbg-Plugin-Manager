// Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#include "updategitprocess.h"

bool _sort_mdata(const Utils::MDATA &mdata1, const Utils::MDATA &mdata2)
{
    bool bResult = false;

    if (mdata1.sName == "x64core") {
        bResult = true;
    } else if (mdata2.sName == "x64core") {
        bResult = false;
    } else {
        bResult = (mdata2.sUpdated > mdata1.sUpdated);
    }

    return bResult;
}

UpdateGitProcess::UpdateGitProcess(QObject *pParent) : QObject(pParent)
{
    bIsStop = false;
    g_bInit = false;
    currentStats = {};
}

void UpdateGitProcess::setData(QString sServerListFileName, QString sServerLastestListFileName, bool bInit)
{
    this->sServerListFileName = sServerListFileName;
    this->sServerLastestListFileName = sServerLastestListFileName;
    this->g_bInit = bInit;
}

void UpdateGitProcess::stop()
{
    bIsStop = true;
}

Utils::STATS UpdateGitProcess::getCurrentStats()
{
    return currentStats;
}

void UpdateGitProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    bIsStop = false;

    QList<Utils::MDATA> listMData = Utils::getModulesFromJSONFile(sServerLastestListFileName);

    if (g_bInit) {  // The first core only
        QList<Utils::MDATA> _listMData;

        if (listMData.count()) {
            _listMData.append(listMData.at(0));

            listMData = _listMData;
        }
    } else {
        if (XBinary::isFileExists(sServerListFileName)) {
            QList<Utils::MDATA> _listMData = Utils::getModulesFromJSONFile(sServerListFileName);

            int nCount = listMData.count();

            for (int i = 0; i < nCount; i++) {
                QString sName = listMData.at(i).sName;
                Utils::MDATA mdata = Utils::getMDataByName(&_listMData, sName);

                if (mdata.sUpdated != "") {
                    listMData[i].sUpdated = mdata.sUpdated;
                }
            }
        }
    }

    std::sort(listMData.begin(), listMData.end(), _sort_mdata);

    int nCount = listMData.count();

    int nNumbersOfGithub = 0;

    for (int i = 0; i < nCount; i++) {
        if (listMData.at(i).sGithub != "") {
            nNumbersOfGithub++;
        }
    }

    currentStats.nTotalModule = nNumbersOfGithub;

    for (int i = 0; (i < nCount) && (!bIsStop); i++) {
        Utils::MDATA mdata = listMData.at(i);

        if (mdata.sGithub != "") {
#ifdef QT_DEBUG
            qDebug("Guthub %s", mdata.sGithub.toLatin1().data());
#endif

            QString sGithub = mdata.sGithub;
            sGithub = sGithub.section("github.com/", 1, 1);

            QString sUserName = sGithub.section("/", 0, 0);
            QString sRepoName = sGithub.section("/", 1, 1);

            XGithub github(sUserName, sRepoName);
            github.setCredentials(sAuthUser, sAuthToken);

            connect(&github, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

            XGithub::RELEASE_HEADER release = github.getLatestRelease(true);

            if (release.bValid) {
                mdata.sDate = release.dt.toString("yyyy-MM-dd");  // rewrite

                if (release.sTag != "") {
                    mdata.sVersion = release.sTag;
                } else {
                    mdata.sVersion = release.sName;
                }

                int nNumberOfAsserts = release.listRecords.count();

                QSet<QString> stDownloads;

                for (int j = 0; j < nNumberOfAsserts; j++) {
                    if (Utils::checkPattern(release.listRecords.at(j).sSrc, &mdata)) {
                        stDownloads.insert(release.listRecords.at(j).sSrc);
                    }
                }

                QList<QString> listStrings = XGithub::getDownloadLinks(release.sBody);

                int nNumberOfLinks = listStrings.count();

                for (int j = 0; j < nNumberOfLinks; j++) {
                    if (Utils::checkPattern(listStrings.at(j), &mdata)) {
                        stDownloads.insert(listStrings.at(j));
                    }
                }

                mdata.listDownloads = stDownloads.toList();

                Utils::updateJsonFile(sServerLastestListFileName, QList<Utils::MDATA>() << mdata);
            }

            if (release.bNetworkError) {
#ifdef QT_DEBUG
                qDebug("Broken %s", mdata.sGithub.toLatin1().data());
#endif
                break;
            }
        }

        currentStats.nCurrentModule = i + 1;
    }

    emit completed(elapsedTimer.elapsed());
}

void UpdateGitProcess::setCredentials(QString sUser, QString sToken)
{
    sAuthUser = sUser;
    sAuthToken = sToken;
}
