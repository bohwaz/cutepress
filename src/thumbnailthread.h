#ifndef THUMBNAILTHREAD_H
#define THUMBNAILTHREAD_H

#include <QThread>
#include <QStringList>
#include <QFileInfoList>
#include <QDesktopServices>
#include <QUrl>
#include <QCryptographicHash>
#include <QSize>
#include <QFile>
#include <QImageReader>
#include <QImageWriter>
#include <QDebug>
#include "qexifimageheader.h"

class ThumbnailThread : public QThread
{
    Q_OBJECT

public:
    ThumbnailThread(QObject* parent = 0);
    ~ThumbnailThread();

public:
    const QFileInfoList *files;
    QStringList thumbFiles;
    QList<int> thumbRotations;

    void setFileList(const QFileInfoList *ifiles);

    void run();
    void Cancel();
    inline bool IsCanceled() { return iThreadCancelled; }
    void clearFiles();


private:
    void createThumbnail(const QString& filename,
                         const QString& thumbname,
                         const int rotaion);

private:
    volatile bool iThreadCancelled;
    /**
     * Image reader for creating thumbnails
     */
    QImageReader *imgReader;
    /**
     * Image writer for creating thumbnails
     */
    QImageWriter *imgWriter;
    QString thumbDir;
    QString fileName;
    QString thumbName;
    int thumbRotation;
};

#endif // THUMBNAILTHREAD_H
