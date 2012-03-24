#include "thumbnailthread.h"

ThumbnailThread::ThumbnailThread(QObject* parent)
    : QThread(parent)
    , iThreadCancelled(false)
{
    imgReader = new QImageReader();
    imgWriter = new QImageWriter();
}

ThumbnailThread::~ThumbnailThread()
{

}


void ThumbnailThread::setFileList(const QFileInfoList *ifiles)
{
    files = ifiles;
//    thumbFiles = ithumbfiles;
//    thumbRotations = ithumbrotations;
}

void ThumbnailThread::clearFiles()
{
    files = NULL;
    //files.clear();
    thumbFiles.clear();
    thumbRotations.clear();
}

void ThumbnailThread::run()
{
    qDebug()<<"ThumbThread Started";
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    thumbDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache/";
#else
    thumbDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache/";
#endif
    for(int i=0;i<files->count();i++) {
        if(iThreadCancelled) { return; }
        fileName = files->at(i).absoluteFilePath();
        thumbName = thumbDir+QCryptographicHash::hash(fileName.toUtf8(), QCryptographicHash::Md5).toHex()+".jpg";
        if(!QFile::exists(thumbName)) {
            if(QExifImageHeader(fileName).value(QExifImageHeader::Orientation).toShort() == 6)
                thumbRotation = 90;
            else if(QExifImageHeader(fileName).value(QExifImageHeader::Orientation).toShort() == 3)
                thumbRotation = 180;
            else if(QExifImageHeader(fileName).value(QExifImageHeader::Orientation).toShort() == 8)
                thumbRotation = 270;
            else
                thumbRotation = 0;

            createThumbnail(fileName, thumbName, thumbRotation);
        }
    }
}

void ThumbnailThread::createThumbnail(const QString &filename, const QString &thumbname, const int rotaion)
{
        if(!QFile::exists(thumbname)){
            imgReader->setFileName(filename);
            imgReader->setScaledSize(QSize(128,128));
            QTransform rot;
            rot.rotate(rotaion);
            QImage temp = imgReader->read().transformed(rot, Qt::FastTransformation);
            imgWriter->setFileName(thumbname);
            imgWriter->setFormat("jpg");
            qDebug()<<"write"<<filename<<thumbname<<imgWriter->write(temp)<<imgWriter->errorString();
        }  //<----else part to delete thumbnail cache ---->
}

void ThumbnailThread::Cancel()
{
    iThreadCancelled = true;
}

