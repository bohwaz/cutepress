#include "WPData.h"

#include <QDesktopServices>
#include <QFileInfo>

#include "md5.h"
#include "utils.h"

/** Name of default gravatar */
static const char* DefaultAvatarPath = ":/images/default-avatar.png";

WPDataGravatars::WPDataGravatars (WPNetworkEngine *engine) :
        iDefaultGravatar(DefaultAvatarPath)
{
    // Resize default image if necessary
    QSize gravatarSize(Size(), Size());
    if(iDefaultGravatar.size() != gravatarSize)
        iDefaultGravatar = iDefaultGravatar.scaled(gravatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // connect signals
    connect(
        this,       SIGNAL(RequestGravatar (QByteArray, int)),
        engine,       SLOT(GetGravatar (QByteArray, int))
    );

    connect(
        engine,     SIGNAL(GravatarDownloaded (QByteArray)),
        this,         SLOT(GravatarDownloaded (QByteArray))
    );
    connect(
        engine,     SIGNAL(GravatarNotFound (QByteArray)),
        this,         SLOT(GravatarNotFound (QByteArray))
    );
    connect(
        engine,     SIGNAL(GravatarFailed (QByteArray)),
        this,         SLOT(GravatarFailed (QByteArray))
    );

    // we don't actually need to keep engine around
}

void WPDataGravatars::GravatarDownloaded (QByteArray hash)
{
    // map back to email
    QString email = iActiveRequests.take(hash);

    if (email.isNull())
        // XXX: wut
        return;

    QPixmap pixmap;

    // load
    if (!LoadGravatar(hash, pixmap))
        // XXX: wut
        pixmap = iDefaultGravatar;

    // yay
    emit GravatarUpdated(email, pixmap);
}

void WPDataGravatars::GravatarNotFound (QByteArray hash)
{
    // mark
    iNegativeCache += hash;
}

void WPDataGravatars::GravatarFailed (QByteArray hash)
{
    (void) hash;

    // just retry next time... ?
}

bool WPDataGravatars::LoadGravatar (const QByteArray &hash, QPixmap &pixmap)
{
    QString path;

// XXX: duplicated functionality from WPDataUsersBlogs::OpenLocalData
#ifdef Q_OS_SYMBIAN
    // from cwd
    path = "";
#else
    // app data dir
    path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    path += ".wordpress/";
#endif

    // hash + suffix
    path += hash + ".jpg";
    
    // Check file existance
    QFileInfo fileInfo(path);
    if(!fileInfo.exists()) { return false; }
    
    // ...modtime
    QDateTime modtime(fileInfo.lastModified());
    modtime = modtime.addDays(7); // Update once a week
    if(modtime < QDateTime::currentDateTime()) { return false; }
    
    // ...loading
    if(!pixmap.load(path)) { return false; }
    
    // ...and size
    if(pixmap.size().width() != Size()) {
        pixmap = QPixmap(); // unload
        return false;
    }
    
    // All ok!
    return true;
}
    
void WPDataGravatars::FetchGravatar (const QString &email, const QByteArray &hash)
{
    // record email so we can later emit GravatarUpdated()
    iActiveRequests.insert(hash, email);

    // issue request
    emit RequestGravatar(hash, Size()); 
}
    
QString WPDataGravatars::NormalizeEmail (const QString &email)
{
    return email.trimmed().toLower();

}

QByteArray WPDataGravatars::HashEmail (const QString &email)
{
    // use QByteArray function from md5.h
    return MD5Hash(email.toAscii());
}

int WPDataGravatars::Size()
{
    static const int size = Utils::smallScreen() ? 50 : 70; 
    return size;
}


QPixmap WPDataGravatars::GetGravatar (const QString &email)
{
    QPixmap ret;

    // compute hash
    QByteArray hash = HashEmail(email);

    // in-memory cache?
    if (iNegativeCache.contains(hash))
        // negative hit
        return iDefaultGravatar;

    // already fetching?
    if (iActiveRequests.contains(hash))
        // busy loading, will return later
        return iDefaultGravatar;

    // local?
    if (LoadGravatar(hash, ret))
        // positive hit
        return ret;

    // fetch from network
    FetchGravatar(email, hash);

    // return placeholder while waiting
    return iDefaultGravatar;
}

QPixmap WPDataGravatars::GetGravatar (WPComment comment)
{
    return GetGravatar(NormalizeEmail(comment->iAuthorEmail));
}
