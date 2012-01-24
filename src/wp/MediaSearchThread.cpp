#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include "MediaSearchThread.h"
#include "WpMediaFile.h"

MediaSearchThread::MediaSearchThread(QObject* parent)
    : QThread(parent)
    , iThreadCancelled(false)
    , iCurrentType(SearchNone)
{
}

MediaSearchThread::~MediaSearchThread()
{

}

void MediaSearchThread::setSearchType(SearchType type)
{
    iCurrentType = type;
    
    iFilters.clear();
    switch(type) {
    case SearchImages:
        WPMediaFile::wildcardExtensionFilters(WPMediaFile::MEDIA_TYPE_IMAGE, iFilters);
        break;
    case SearchVideos:
        WPMediaFile::wildcardExtensionFilters(WPMediaFile::MEDIA_TYPE_VIDEO, iFilters);
        break;
    case SearchNone:
        break;
    }
    
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN)
    switch(type) {
    case SearchImages:
        iSearchDirs << QDir("C:\\Images");
        iSearchDirs << QDir("E:\\Images");
        break;
    case SearchVideos:
        iSearchDirs << QDir("C:\\Videos");
        iSearchDirs << QDir("E:\\Videos");
    case SearchNone:
        break;
    }
#else
    iSearchDirs << QDir("/home/user/MyDocs/DCIM");
    iSearchDirs << QDir("/home/user/MyDocs/.camera");
    iSearchDirs << QDir("/media/mmc1/");
    iSearchDirs << QDir("/home/user/MyDocs/Pictures/");//QDir(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    
    switch(type) {
    case SearchImages:
        iSearchDirs << QDir("/home/user/MyDocs/.images");
        break;
    case SearchVideos:
        iSearchDirs << QDir("/home/user/MyDocs/.videos");
        break;
    case SearchNone:
        break;
    }
#endif
}

void MediaSearchThread::run()
{
    Q_ASSERT(iCurrentType != SearchNone);
    
    for(DirList::iterator it = iSearchDirs.begin(); it != iSearchDirs.end(); ++it) {
        if(iThreadCancelled) { return; }
        ParseDir(*it);
    }
}

void MediaSearchThread::ParseDir(const QDir& aDir)
{
    QFileInfoList contents(aDir.entryInfoList(iFilters, QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files));

    const int count = contents.size();
    for (int i = 0; i < count; i++)
    {
        if (iThreadCancelled)
            return;
        if (contents[i].isDir())
            ParseDir(contents[i].filePath());
        else {
            iFileInfos.append(contents[i]);
            qDebug()<<contents[i].filePath();
        }
    }
}

void MediaSearchThread::Cancel()
{
    iThreadCancelled = true;
}
