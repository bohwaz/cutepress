#include "utils.h"

#include <QDir>
#include <QImageReader>
#include <QGraphicsLinearLayout>
#include <QDesktopServices>
#include<QDebug>

void Utils::ForceFocus(QGraphicsItem * item)
    {
    if(!item) { return; }
    item->setFocus();
    item->hide();
    item->show();
    item->setFocus();
    }


QSharedPointer<QTemporaryFile> Utils::getTempFileWithMinimumSize(quint64 minSizeInBytes)
    {
    QDir tempPath = QDir::temp();
    
    // XXX check free space. No Qt function for this???
    if(getFreeSpaceInDir(tempPath) < minSizeInBytes)
        {
        return QSharedPointer<QTemporaryFile>();
        }
    
    QString nameTemplate = tempPath.filePath("WordPressTempFileXXXXXX.tmp");
    QSharedPointer<QTemporaryFile> file(new QTemporaryFile(nameTemplate));
    QString name = file->fileName();
    return file;
    }

bool Utils::resizeImage(QString const & filename, QIODevice & output, const QSize& targetSize, const int angle)
    {
    QImageReader reader (filename);

    // get metadata
    QSize imageSize = reader.size();
    
    if (!targetSize.isNull() &&
            (imageSize.height() > targetSize.height() ||
             imageSize.width() > targetSize.width()))
    {
        // scale down
        imageSize.scale(targetSize, Qt::KeepAspectRatio);
        reader.setScaledSize(imageSize);
    }

    // read in scaled QImage
    QImage image = reader.read();

    if (image.isNull())
        // failure
        return false;

    if (angle != 0)
    {
        QMatrix mat;
        mat.rotate(angle);
        image = image.transformed(mat);
    }

    output.open(QIODevice::WriteOnly);
    image.save(&output, "JPG");

    // read back from the temporary image data buffer
    output.seek(0);
    return true;
    }

int Utils::indexOfItem(QGraphicsLinearLayout * layout, QGraphicsLayoutItem * item)
    {
    QGraphicsLayoutItem * layoutItem = 0;
    int i = 0;
    while((layoutItem = layout->itemAt(i)))
        {
        if(item == layoutItem) { return i; }
        ++i;
        }
    return -1;
    }

QString Utils::toDisplayHtml(QString const & string)
    {
    QString html(string);

    html.replace("\n", "<br />");
    
    // "The title of the blog is not showed properly.
    // It contains the entity #39 in it
    // (I know this is an issue on the WP server side,
    // but you should remove the double encoded entity 
    // on the client side as other mobile apps)"
    html.replace("&#039;", "'");

    return html;
    }

bool Utils::openLocalDataFile (QFile &file, QFile::OpenMode mode)
{
    // prefix to app data dir
    QString path;

#ifdef Q_OS_SYMBIAN
    // store in CWD...
    path = "";

#else
    // store under Qt's DataLocation
    path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    // inour application's directory
    path += ".cutepress/";

    QDir dir(path);

    if (!dir.exists())
        dir.mkpath(path);

#endif

    // use basename from file
    path += file.fileName();

    // set full path
    file.setFileName(path);
    qDebug()<<file.fileName();

    // open
    return file.open(mode);
}

/*************** Platform dependent functions below ******************/

#if defined(Q_OS_SYMBIAN)

#include <f32file.h>

quint64 Utils::getFreeSpaceInDir(QDir dir)
    {
    RFs fs;
    fs.Connect();
    
    TChar drive(dir.rootPath()[0].toAscii());
    TInt driveNum;
    TInt err = fs.CharToDrive(drive, driveNum); 
    
    quint64 free = 0;
    if(err == KErrNone)
        {
        TVolumeInfo volInfo;
        err = fs.Volume(volInfo, driveNum);
        if(err == KErrNone)
            {
            free = volInfo.iFree;
            }
        }
    
    fs.Close();
    return free;
    }

#elif defined(Q_WS_MAEMO_5)

#include <sys/statvfs.h>

quint64 Utils::getFreeSpaceInDir(QDir dir)
    {
    struct statvfs info;
    int err = statvfs(dir.absolutePath().toAscii().constData(), &info);
    
    quint64 free = 0;
    if(!err)
        {
        // Check blocks available to non-privileged process
        free = info.f_bsize * info.f_bavail;
        }
    
    return free;
    }

#else

#warning No disk space cheking function available on this platform!  
#include <limits>

quint64 Utils::getFreeSpaceInDir(QDir dir)
    {
    return std::numeric_limits<quint64>::max();
    }

#endif
