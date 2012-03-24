#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QCryptographicHash>
#include "MediaSearchThread.h"
#include "WpMediaFile.h"

MediaSearchThread::MediaSearchThread(QObject* parent)
    : QThread(parent)
    , iThreadCancelled(false)
    , iCurrentType(SearchNone)
{
    imgReader = new QImageReader();
    imgWriter = new QImageWriter();
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
void MediaSearchThread::setSearchDirs(QStringList dirs)
{
    for(int i=0;i<dirs.count();i++) {
        if(!iSearchDirs.contains(dirs.at(i)))
            iSearchDirs << dirs.at(i);
    }
}

void MediaSearchThread::run()
{
    Q_ASSERT(iCurrentType != SearchNone);

    QDir mkd = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    if(!QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache").exists())
        mkd.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache");
#else
    qDebug()<<"Cache directory exists?"<<QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache").exists()<<QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache";
    if(!QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache").exists())
        qDebug()<<"Making cache directory"<<QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache"<<mkd.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache");
#endif
    for(DirList::iterator it = iSearchDirs.begin(); it != iSearchDirs.end(); ++it) {
        if(iThreadCancelled) { return; }
        ParseDir(*it);
    }
    for(int i=0; i<iFileInfos.count(); i++){
        for(int j=i+1; j<iFileInfos.count(); j++){
            if(iFileInfos.at(i).created()<iFileInfos.at(j).created()){
                iFileInfos.swap(i, j);
            }
        }
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
            iFileInfos.append(contents[i]);/*
            QString type = contents[i].suffix();
            type = type.toLower();
            if(type=="jpg"||type=="jpeg"||type=="png"||type=="bmp") {
                                int rotation;
                if(QExifImageHeader(contents[i].absoluteFilePath()).value(QExifImageHeader::Orientation).toShort() == 6)
                    rotation = 90;
                else if(QExifImageHeader(contents[i].absoluteFilePath()).value(QExifImageHeader::Orientation).toShort() == 3)
                    rotation = 180;
                else if(QExifImageHeader(contents[i].absoluteFilePath()).value(QExifImageHeader::Orientation).toShort() == 8)
                    rotation = 270;
                else
                    rotation = 0;

                QDir mkd = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
                QString thumb;
    #if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
                thumb = QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache/"+
                        QCryptographicHash::hash(contents[i].fileName().toUtf8(), QCryptographicHash::Md5).toHex()+".jpg";
                if(!QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache").exists())
                    mkd.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache");
    #else
                thumb = QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache/"+
                        QCryptographicHash::hash(contents[i].fileName().toUtf8(), QCryptographicHash::Md5).toHex()+".jpg";
                if(!QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache").exists())
                    mkd.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache");
    #endif
                //qDebug()<<thumb<<QFile::exists(thumb);
                if(!QFile::exists(thumb)){
                    qDebug()<<"Create thumbnail!";
                    imgReader->setFileName(contents[i].absoluteFilePath());
                    imgReader->setScaledSize(QSize(128,128));
                    QTransform rot;
                    rot.rotate(rotation);
                    QImage temp = imgReader->read().transformed(rot, Qt::FastTransformation);
                    imgWriter->setFileName(thumb);
                    imgWriter->setFormat("jpg");
                    qDebug()<<"write"<<thumb<<imgWriter->write(temp)<<contents[i].absoluteFilePath()<<imgWriter->errorString();
                }
            }
            qDebug()<<contents[i].filePath();*/
        }
    }
}

void MediaSearchThread::Cancel()
{
    iThreadCancelled = true;
}
