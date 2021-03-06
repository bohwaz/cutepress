#ifndef MEDIASEARCHTHREAD_H
#define MEDIASEARCHTHREAD_H

#include <QThread>
#include <QLinkedList>
#include <QStringList>
#include <QFileInfoList>
#include <QImageReader>
#include <QImageWriter>
#include <QDateTime>
#include "../qexifimageheader.h"

class QDir;
class QFileInfo;

class MediaSearchThread : public QThread
{
    Q_OBJECT

public:
    MediaSearchThread(QObject* parent = 0);
    ~MediaSearchThread();

public:
    
    enum SearchType {
        SearchNone,
        SearchImages,
        SearchVideos
    };
    
    void setSearchType(SearchType type);
    void setSearchDirs(QStringList dirs);
    SearchType searchType() { return iCurrentType; }
    
    void run();
    void Cancel();
    inline bool IsCanceled() { return iThreadCancelled; }

    //typedef QList<QFileInfo const> FileInfoList;
    QFileInfoList const & InfoList() { return iFileInfos; }

private:
    void ParseDir(const QDir& aDir);

private:
    volatile bool iThreadCancelled;
    QFileInfoList iFileInfos;
    
    QStringList iFilters;
    typedef QLinkedList<QDir> DirList;
    DirList iSearchDirs;
    
    SearchType iCurrentType;
    QImageReader *imgReader;
    QImageWriter *imgWriter;
};

#endif // MEDIASEARCHTHREAD_H
