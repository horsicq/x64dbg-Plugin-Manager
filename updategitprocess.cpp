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
#include "updategitprocess.h"

UpdateGitProcess::UpdateGitProcess(QObject *pParent) : QObject(pParent)
{
    bIsStop=false;
    currentStats={};
}

void UpdateGitProcess::setData(QString sServerListFileName)
{
    this->sServerListFileName=sServerListFileName;
}

void UpdateGitProcess::stop()
{
    bIsStop=true;
}

Utils::STATS UpdateGitProcess::getCurrentStats()
{
    return currentStats;
}

void UpdateGitProcess::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    bIsStop=false;

    QList<Utils::MDATA> listMData=Utils::getModulesFromJSONFile(sServerListFileName);

    int nCount=listMData.count();

    int nNumbersOfGithub=0;

    for(int i=0;i<nCount;i++)
    {
        if(listMData.at(i).sGithub!="")
        {
            nNumbersOfGithub++;
        }
    }

    currentStats.nTotalModule=nNumbersOfGithub;

    for(int i=0;(i<nCount)&&(!bIsStop);i++)
    {
        Utils::MDATA mdata=listMData.at(i);

        if(mdata.sGithub!="")
        {
            QString sGithub=mdata.sGithub;
            sGithub=sGithub.section("github.com/",1,1);

            QString sUserName=sGithub.section("/",0,0);
            QString sRepoName=sGithub.section("/",1,1);

            XGithub github(sUserName,sRepoName);

            connect(&github,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));

            XGithub::RELEASE_HEADER release=github.getLatestRelease(true);

            if(release.bValid)
            {
                mdata.sDate=release.dt.toString("yyyy-MM-dd"); // rewrite

                if(release.sTag!="")
                {
                    mdata.sVersion=release.sTag;
                }
                else
                {
                    mdata.sVersion=release.sName;
                }

                int nNumberOfAsserts=release.listRecords.count();

                QSet<QString> stDownloads;

                for(int j=0;j<nNumberOfAsserts;j++)
                {
                    if(Utils::checkPattern(release.listRecords.at(j).sSrc,&mdata))
                    {
                        stDownloads.insert(release.listRecords.at(j).sSrc);
                    }
                }

                QList<QString> listStrings=XGithub::getDownloadLinks(release.sBody);

                int nNumberOfLinks=listStrings.count();

                for(int j=0;j<nNumberOfLinks;j++)
                {
                    if(Utils::checkPattern(listStrings.at(j),&mdata))
                    {
                        stDownloads.insert(listStrings.at(j));
                    }
                }

                mdata.listDownloads=stDownloads.toList();

                Utils::updateJsonFile(sServerListFileName,QList<Utils::MDATA>() << mdata);
            }
            else
            {
                break;
            }
        }

        currentStats.nCurrentModule=i+1;
    }

    emit completed(elapsedTimer.elapsed());
}
