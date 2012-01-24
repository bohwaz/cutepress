/*
 * WPNetworkEngine.h
 *
 *  Created on: 4.11.2009
 *      Author: jkal
 */

#ifndef WPNETWORKENGINE_H_
#define WPNETWORKENGINE_H_

#include <QObject>
#include <QMap>
#include <QSet>
#include <QXmlStreamWriter>
#include <QNetworkRequest>
#include <QSslError>
#include <QDateTime>
#include <QSharedPointer>

#ifdef HAVE_QTMOBILITY
#include <QNetworkSession>
// Fugly, but must be declared for signals and slots to work
QTM_USE_NAMESPACE
#endif // HAVE_QT_MOBILITY


#include "WPDataCustomField.h"
#include "WpMediaFile.h"

class WPNetworkEngine;

/**
 * A remote XMLRPC server we communicate with
 *
 * XXX: move to WPData.h?
 */
struct WPNetworkEndpoint
{
/* URLs XXX: should be QUrl's */
    QString iBlogUrl;
    QString iRpcUrl;

/* Authentication*/
    QString iUsername;
    QString iPassword;
    QString iApiKey;

/* Identifier for multi-blog setups? */
    QString iBlogId;

/* Settings */
    QString iNumberOfPosts;
    bool iResizePhotos;

    bool isWordpressHosted() const {
        QUrl url = QUrl::fromUserInput(iRpcUrl);
        return url.host().endsWith(".wordpress.com");
    }

};

/**
 * TWPNetworkEngineCommand includes all network operations.
 */
enum TWPNetworkEngineCommand
{
    EIdle,
    EGetMainPage,
    EGetRSD,
    EGetUsersBlogs,
    EGetPages,
    EGetPosts,
    EGetCategories,
    EGetTags,
    EGetPageStatuses,
    EGetPostStatuses,
    EGetCommentStatuses,
    EGetComments,
    EGetPage,
    ENewPage,
    EEditPage,
    EDeletePage,
    EGetComment,
    ENewComment,
    EEditComment,
    EDeleteComment,
    EGetPost,
    ENewPost,
    EEditPost,
    EDeletePost,
    EGetGravatar,
    ENewCategory,
    EUploadFile,
    EUploadMultipleFiles,
    EGetStats,
    EReport,
    EFetchingLocation,
    EFetchingApiKey,
    EGetMediaSettings,
    EGetMediaLibrary,
    EOpenNetworkSession
};

#include "WPXmlResponse.h"
#include "WPData.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

// Forward declarations
class QAuthenticator;
class QNetworkReply;
class QNetworkAccessManager;
class QTreeWidget;
class QListWidgetItem;


/**
 * Network request object.
 */
class WPNetworkQueueItem
{
public:
    WPNetworkQueueItem();
    WPNetworkQueueItem(QNetworkReply* aNetworkReply, bool aShowWaitNote, bool aShowErrorNote, int aClientSideItemId = 0,  WPDataBlog* aBlogData = NULL);

    QNetworkReply* iNetworkReply;
    bool        iShowWaitNote;
    bool        iShowErrorNote;
    int         iNetworkReqId;
    WPDataBlog* iBlogData;
    
    QSharedPointer<QIODevice> iPOSTData;
};

/**
 * Track each batch of files we need to upload
 */
struct FileUploadItem
{
    /** List of media files to upload */
    QList<WPMediaFile> files;

    /** List of results to return */
    QList<WPMediaFile> results;

    /** Tracking ID */
    int localId;
};

/**
 * WPEngine that handles network side logic so that UI can focus on UX.
 */
class WPNetworkEngine: public QObject
{
    Q_OBJECT

public:
    enum TGetCommentsStatus
    {
        EAllComments,
        EPendingComments
    };

public:
    // Constructors
    WPNetworkEngine();
    virtual ~WPNetworkEngine();

public slots:
    /**
     * Switch the currently used endpoint blog for requests to the given endpoint.
     *
     * The endpoint must have a known iRpcUrl
     */
    void SetEndpoint (WPNetworkEndpoint endpoint);
    
    /**
     * Autodiscover a new blog with an incomplete WPNetworkEndpoint.
     *
     * This will emit a BlogsListed with the base endpoint info used and the full list of blogs
     * once done.
     *
     * Note that after this incomplete endpoint has been discovered, a new, full endpoint MUST
     * be set using SetEndpoint() before issuing any more requests, since the endpoint set here
     * does not contain all required info.
     */
    void DiscoverEndpoint (WPNetworkEndpoint endpoint);

signals: /* General */

    /**
     * Discovery on the given endpoint completed. The following list of blog items contains the
     * additional endpoint info for each blog found at the given endpoint.
     */
    void BlogsListed (WPNetworkEndpoint endpoint, QList<WPXmlItem> blogList);

/* Internal methods */
private:
    /**
     * If body is given, send a POST request to the given URL, otherwise, send a GET request.
     *
     * A new WPNetworkQueueItem is created with an unique ID, and stored in the request/response
     * queue along with the QNetworkReply we are waiting for.
     *
     * Additionally, a TaskStarted() signal is emitted.
     *
     * The return value is the per-session unique request ID.
     */
    int ExecuteRequest (const QUrl &url, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body = QSharedPointer<QIODevice>(), const QString &contentType = QString());

    /**
     * As above, but use the given blog's RPC API URL.
     */
    int ExecuteRequest (const WPNetworkEndpoint &endpoint, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body = QSharedPointer<QIODevice>(), const QString &contentType = QString());

    /**
     * Build an XMLRPC methodCall with the given set of parameters, and execute it.
     */
    int ExecuteCustomRequest (TWPNetworkEngineCommand cmd, int id,
        const QString &method,
        const QStringList *params,
        const QMap<QString, QString> *content,
        const QMap<QString, QStringList> *listContent,
        const QList<WPDataCustomField> *customFields,
        const QString *suffix,
        const QList<QString> *arrayContent);

    /**
     * Build an XMLRPC methodCall starting with the id/username/password params for the current
     * blog, in addition to the given parameters.
     */
    int ExecuteGenericRequest (TWPNetworkEngineCommand cmd, int id,
            const QString &method,
            const QString *objectId = NULL, 
            const QMap<QString, QString> *content = NULL,
            const QMap<QString, QStringList> *listContent = NULL,
            const QList<WPDataCustomField> *customFields = NULL,
            const QString *suffix = NULL );

/* Stubs for ExecuteGenericRequest */
    int ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
            const QString &method );

    int ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
            const QString &method,
            const QString &objectId );

    int ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
            const QString &method,
            const QMap<QString, QString> &object );

    int ExecuteSimpleRequest (TWPNetworkEngineCommand cmd,
            const QString &method,
            const QString &objectId,
            const QMap<QString, QString> &object );


private slots:
#ifndef QT_NO_OPENSSL
    /**
     * SSL errors
     */
    void HandleSslErrors (QNetworkReply *, const QList<QSslError> &);
#endif // QT_NO_OPENSSL

    /**
     * Request finished, got reply
     */
    void HandleNetworkReply (QNetworkReply *networkReply);


    void HandleAuthentication(QNetworkReply *networkReply, QAuthenticator *authenticator);

private:
    /* Error messages */

    /**
     * Map an XMLRPC fault code to an UI message
     */
    QString FaultToError (int faultCode, QString faultString);
    QString ParseResponseToError (WPXmlResponse &response);
    QString NetworkReplyToError (QNetworkReply *networkReply);

    /**
     * Request failed with given error, handle and/or report.
     */
    void HandleNetworkError (QNetworkReply *networkReply, WPNetworkQueueItem &item, TWPNetworkEngineCommand command, const QString &message);

    /**
     * Got a valid XMLRPC response, handle it.
     *
     * Return false if parsing the response data fails.
     */
    bool HandleXmlResponse (WPXmlResponse &response,  WPNetworkQueueItem &item,                      TWPNetworkEngineCommand command);

signals: /* -> UI */

    /**
     * Request was issued.
     */
    void TaskStarted (TWPNetworkEngineCommand command);

    /**
     * Request was finished succesfully.
     */
    void TaskFinished (TWPNetworkEngineCommand command);

    /**
     * Request failed.
     */
    void TaskFailed (TWPNetworkEngineCommand command, QString msg);
    
    void NoAccessPoints();

public slots:
/* Pages */
    void GetPages ();
    void GetPage (int localId, QString itemId);
    void NewPage (int localId, WPXmlItem item);
    void EditPage (int localId, QString itemId, WPXmlItem item);
    void DeletePage (int localId, QString pageId);
    
/* Posts */
    void GetPosts ();
    void GetPost (int localId, QString itemId);
    void NewPost (int localId, WPXmlItem item);
    void EditPost (int localId, QString itemId, WPXmlItem item);
    void DeletePost (int localId, QString itemId);

/* 
 * Comments
 *
 * XXX: object state doesn't work exactly the same way as Page/Post... 
 */
    void GetComments (TGetCommentsStatus aStatus = EAllComments,
                     const QString& aPostId = QString(), int aNumberOfComments = 30, int aOffset = 0);
    void GetComment (int localId, QString itemId);

    /** Uses post_id, comment_parent, content */
    void NewComment (int localId, WPXmlItem item);

    /** Uses status */
    void EditComment (int localId, QString itemId, WPXmlItem item);
    void DeleteComment (int localId, QString itemId);

    /* Files */
    void UploadFiles (int localId, QList<WPMediaFile> files);
    /* File */
    void UploadFile (WPMediaFile *file);
    
    /* Gravatars */
    void GetGravatar (QByteArray hash, int size);

/* Others */
    // categories
    void GetCategories ();
    void NewCategory (int localId, WPXmlItem item);

    void GetMedia();

    void GetTags ();
    void GetPageStatuses ();
    void GetPostStatuses ();
    void GetCommentStatuses ();
    void GetStats (QString table = QString(), int days = 7);
    void GetApiKey();
    void GetMediaSettings();
    void ReportStatsIfNeeded (int blogCount);

    void GetLocationAddress(qreal latitude, qreal longitude);

signals:

    /* Pages */
    void PagesListed (QList<WPXmlItem> itemList);
    void PageGotten (int localId, WPXmlItem item);
    void PageCreated (int localId, QString itemId);
    void PageEdited (int localId);
    void PageDeleted (int localId);
    void PageFailed (int localId, TWPNetworkEngineCommand command);
    
    /* Posts */
    void PostsListed (QList<WPXmlItem> itemList);
    void PostGotten (int localId, WPXmlItem item);
    void PostCreated (int localId, QString itemId);
    void PostEdited (int localId);
    void PostDeleted (int localId);
    void PostFailed (int localId, TWPNetworkEngineCommand command);

    /** Comments */
    void CommentsListed (QList<WPXmlItem> itemList);
    void CommentGotten (int localId, WPXmlItem item);
    void CommentCreated (int localId, QString itemId);
    void CommentEdited (int localId);
    void CommentDeleted (int localId);
    void CommentFailed (int localId, TWPNetworkEngineCommand command);

    /* Files */
    void FilesUploaded (int localId, QList<WPMediaFile> files);
    /* Files */
    void FileUploaded (WPMediaFile);
    /* Files */
    void FileUploadFailed (WPMediaFile);

    /* Gravatars */
    void GravatarDownloaded (QByteArray hash);
    void GravatarNotFound (QByteArray hash);
    void GravatarFailed (QByteArray hash);

    /* Categories */
    void CategoriesListed (QList<WPXmlItem> itemList);
    void CategoryCreated (int localId, QString itemId);

    /* Media */
    void MediaListed (QList<WPXmlItem> itemList);

    /* Stats */
    void ApiKeyReceived(const QString &apiKey);
    /** We were able to request the given blog's stats using the given api key */
    void GetStatsSucceeded (QString blogUrl, QString apiKey);

    void FormattedAddressGotten(qreal latitude, qreal longitude, const QString &formattedAddress);

public:
    // XXX: for switching blogs
    void ClearRequestQueue();

    // XXX: legacy time get&formatters (unclear timezones)
    static QString LocalizedTime(const QDateTime& aTimeString);
    static QString LocalizedTime(const QString& aTimeString);
    static QString ShortLocalizedTime(const QString& aTimeString);
    static QString ISO8601Time(const QDateTime& aDateTime);

public slots: /* XXX: legacy signals <- UI */
    
signals: /* XXX: legacy signals -> UI */
    // categories
    void UpdateCategoryData(const WPDataBlog& aBlog);

    // stats
    void UpdateStats(const WPDataStats& aStats);

private:
    // Functions that get data from network
    void GetUsersBlogs(const QUrl& aUrl);


    static QNetworkRequest CreateNetworkRequest(const QUrl& aUrl, TWPNetworkEngineCommand aType);
    
    /**
     * Find and return the hexadecimal hash component from the given gravatar response URL, discarding
     * any possible .suffix
     */
    static QByteArray ParseHashFromGravatarUrl (const QUrl &url);

    // Gets next uniq id for network operations.
    // Utilized to map between network request and object ids.
    int NextUniqId();

    // XXX: remove
    bool ValidBlog (const WPDataBlog* aBlogData)
    {
        (void) aBlogData;

        return true;
    }
     
/** File Uploads */
private:
    /**
     * Queued FileUploadItems, sent out one at a time
     */
    QList<FileUploadItem> iFileUploadQueue;
    /**
     * Queued FileUploadItem, sent out one at a time
     */
    WPMediaFile *iFileUploadItem;
    
    /**
     * Enqueue a FileUploadItem for uploading, starting the first upload if appropriate.
     */
    void EnqueueFileUploads (FileUploadItem &item);

    /**
     * Go through the file upload queue, and upload any remaining file on the current item,
     * or notify completion and continue on to the next item, if any.
     *
     * Handles possible errors from ExecuteFileUpload by skipping that file.
     */
    void NextFileUpload ();

    /**
     * Prepare the image/file to be uploaded, and then create a request and send it.
     */
    bool ExecuteFileUpload (int localId, const WPMediaFile &fileinfo);

    /**
     * Generate the XML request body for a file upload request, and execute it.
     *
     * This calls ExecuteRequest to emit TaskStarted etc.
     */
    void ExecuteFileUploadRequest (int localId, const WPMediaFile &file, QIODevice *src);
    
public:    
    // XXX: remove: current blog
    // used to update auxilliary data fields from network req's (stats, tags, categories, statuses)
    WPDataBlog* iBlogData;

private:
    // Session management
    
    void OpenNetworkSession();
    bool QueueRequestIfNecessary(const QUrl &url, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body, const QString &contentType);
    void ExecuteQueuedRequests();
    
    struct QueuedNetworkRequest {
        QueuedNetworkRequest(const QUrl &url, TWPNetworkEngineCommand cmd, int id, QSharedPointer<QIODevice> body, const QString &contentType)
          : url(url), cmd(cmd), id(id), body(body), contentType(contentType) {}
    
        QUrl url;
        TWPNetworkEngineCommand cmd;
        int id;
        QSharedPointer<QIODevice> body;
        QString contentType;
    };
    
    QList<QueuedNetworkRequest> iQueuedRequests;
    
#ifdef HAVE_QTMOBILITY
    QNetworkSession * iNetworkSession;
    
private slots:
    void SessionStateChanged(QNetworkSession::State state);
#endif // HAVE_QTMOBILITY
    
private:
    /** Qt engine for HTTP get/post */
    QNetworkAccessManager* iNetworkAccessManager;

    /** Queue of pending requests/replies */
    QList<WPNetworkQueueItem> iCurrentRequests;

    /** Currently used blog endpoint */
    WPNetworkEndpoint iEndpoint;
    
    // Engine book keeping
    int     iUniqReqId;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif /* WPNETWORKENGINE_H_ */
