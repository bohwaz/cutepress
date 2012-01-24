/*
 * WPData.h
 *
 *  Created on: 5.11.2009
 *      Author: jkal
 */

#ifndef WPDATA_H_
#define WPDATA_H_

#include <QObject>
#include <QDebug>
#include <QDate>
#include <QList>
#include <QPair>
#include <QMap>
#include <QFile>
#include <QDataStream>
#include <QStringList>
#ifdef HAVE_QTM_LOCATION
#include <QGeoPositionInfo>
#endif // HAVE_QTM_LOCATION
#include <QPixmap>

class WPDataUsersBlogs;
class WPDataStats;
class WPDataBlog;
class WPDataPage;
class WPDataPost;
class WPDataCategory;
class WPDataTag;
class WPDataComment;
class WPDataMediaItem;
class WPNetworkQueueItem;

/**
 * Refcounted pointer type for safe persistent references to data objects.
 *
 * Note that this type behaves as a pointer, and its members must be derefenced using the
 * -> operator, or cast to a reference.
 */
typedef QExplicitlySharedDataPointer<WPDataPage> WPPage;
typedef QExplicitlySharedDataPointer<WPDataPost> WPPost;
typedef QExplicitlySharedDataPointer<WPDataComment> WPComment;

/**
 * Container class for WPDataItems
 */
template <class DataItemType> class WPDataItemCollection;

#include "WPNetworkEngine.h"

/*
 * Global data schema version number.
 *
 * Incremented for each and every schema update in each object type below that breaks
 * forwards-compatibility.
 *
 * We refuse to load data with a higher (i.e. unknown) global version number.
 */
const int KDataUsersBlogsVersionNumber = 3;

/* Incremented for each schema update in the corresponding object */
const int KDataBlogVersionNumber = 4;
const int KDataPostVersionNumber = 3;
const int KDataPageVersionNumber = 2;
const int KDataCategoryVersionNumber = 1;
const int KDataMediaItemVersionNumber = 1;
const int KDataTagVersionNumber = 1;
const int KDataCommentVersionNumber = 1;
const int KDataStatsVersionNumber = 3;

/**
 * Gravatar caching and downloading
 */
class WPDataGravatars : public QObject
{
    Q_OBJECT

public:
    /**
     * Use the given NetworkEngine to download the gravatars
     */
    WPDataGravatars (WPNetworkEngine *engine);

private:
    
    /** The default gravatar that we use */
    QPixmap iDefaultGravatar;

    /** Gravatar hashes that are known to not exist */
    QSet<QByteArray> iNegativeCache;

    /** Currently active requests, hash -> email */
    QMap<QByteArray, QString> iActiveRequests;

signals: /* -> Network */
    void RequestGravatar (QByteArray hash, int size);
    
private slots: /* Network -> */
    void GravatarDownloaded (QByteArray hash);
    void GravatarNotFound (QByteArray hash);
    void GravatarFailed (QByteArray hash);

signals: /* -> UI */
    /**
     * New gravatar available for the given email
     */
    void GravatarUpdated (const QString &email, const QPixmap &pixmap);

private:
    /**
     * Attempt to load the local pixmap from the given hash
     */
    bool LoadGravatar (const QByteArray &hash, QPixmap &pixmap);

    /**
     * Fetch gravatar from network, issuing a request
     */
    void FetchGravatar (const QString &email, const QByteArray &hash);

public:    
    /**
     * Normalize the email address for comparisons
     */
    static QString NormalizeEmail (const QString &email);

    /**
     * Compute the hash for the given normalized email
     */
    static QByteArray HashEmail (const QString &email);
    
    /**
     * Get gravatar size. Gravatars are always square
     */
    static int Size();

    /**
     * Look up and return a QPixmap representing the currently known gravatar for the given 
     * NORMALIZED email address.
     *
     * The gravatar may be updated at a later time, emitting a GravatarUpdated() signal with the
     * new pixmap.
     */
    QPixmap GetGravatar (const QString &email);
    
    /**
     * Look up gravatar for given comment's email address
     */
    QPixmap GetGravatar (WPComment comment);
};

/**
 * The top-level data model object.
 *
 * There is only one WPDataUsersBlogs object, which holds the collection of WPDataBlogs, and
 * the currently active WPBlog.
 */
class WPDataUsersBlogs : public QObject
{
    Q_OBJECT

public:
    /**
     * Use the given WPNetworkEngine to communicate with the server
     */
    WPDataUsersBlogs (WPNetworkEngine *engine);

    /** Cleanup */
    ~WPDataUsersBlogs ();

    /**
     * Attempt to open up the local data file, and read in our serialized state.
     *
     * Returns true if we managed to load some set of blogs, otherwise false.
     */
    bool LoadLocalData ();

    /**
     * Open up our local data file and serialize our state into it.
     */
    void SaveLocalData ();

    /**
     * Retrieve a WPBlog by LocalId
     */
    WPDataBlog* GetBlog (const QString &localId);

    /**
     * Returns the currently active blog
     */
    WPDataBlog* GetCurrentBlog ();

    /**
     * Read-only list of blogs
     */
    const QList<WPDataBlog *> & GetBlogs () const
    {
        return iBlogs;
    }

    int GetBlogCount () const
    {
        return iBlogs.size();
    }

    /**
     * Switches the currently active blog to the given blog.
     *
     * The given blog must not be NULL
     */
    void OpenBlog (WPDataBlog *newBlog);

    /**
     * Remove the given blog from our list of blogs, opening the next blog in its place.
     */
    bool DeleteBlog (WPDataBlog *blog);

    /**
     * Change the network settings for the given blog.
     *
     * Note that the iBlogUrl field MAY NOT be changed.
     */
    void EditBlog (WPDataBlog *blog, const WPNetworkEndpoint &endpoint);

    /**
     * Attempt to add a new blog with the given settings.
     *
     * This will deactivate the currently active blog, if any.
     *
     * This will first attempt to discover the XML-RPC URL for the given blog, and query for
     * a full list of the blogs available. A new WPDataBlog will then be created for each of those
     * blogs, and the first one will be activated.
     */
    void NewBlog (const WPNetworkEndpoint &endpoint);

    /**
     * Refresh the data for the currently active blog, eventually emitting a series of
     * UpdateItemDataFull signals.
     *
     * Also handles some bookkeeping.
     */
    void RefreshCurrentBlog ();

signals: /* -> Network */
    void SetEndpoint (WPNetworkEndpoint endpoint);
    void DiscoverEndpoint (WPNetworkEndpoint endpoint);
    void ReportStatsIfNeeded (int blogCount);

private slots:
    /**
     * Blogs listing from server following DiscoverEndpoint from NewBlog.
     */
    void BlogsListed (WPNetworkEndpoint endpoint, QList<WPXmlItem> blogList);

    /**
     * Blog's API key was updated, apply new endpoint with given api key and refresh stats if active
     */
    void BlogApiKeyUpdated (WPDataBlog *blog, QString apiKey);

signals: /* -> UI */

    /** Something failed... */
    void DataError (const QString &error);

    /** Updated list of blogs, initial blog setup completed... */
    void GetUserBlogsCompleted (const WPDataBlog &blog);

    /** List of blogs changed */
    void UpdateBlogs (const WPDataUsersBlogs &blogs);

// page    
    /** Any number of Pages on the given Blog were updated */
    void UpdatePageDataFull (const WPDataBlog &blog);
    
    /** One page on the given blog was updated */
    void UpdatePageData (const WPDataBlog &blog);
    
// post    
    /** Any number of Posts on the given Blog were updated */
    void UpdatePostDataFull (const WPDataBlog &blog);
    
    /** One post on the given blog was updated */
    void UpdatePostData (const WPDataBlog &blog);

// comments   
    /** Any number of Comments on the given Blog were updated */
    void UpdateCommentDataFull (const WPDataBlog &blog);

    /** One comment on the given blog was updated */
    void UpdateCommentData (const WPDataBlog &blog);
    
// categories    
    void UpdateCategoryData (const WPDataBlog &blog);

// media
    void UpdateMediaData (const WPDataBlog &blog);

private:

    /** Grand list of blogs */
    QList<WPDataBlog *> iBlogs;

    /** WPNetworkEngine we're using */
    WPNetworkEngine *engine;

    /** Currently active blog */
    WPDataBlog *iActiveBlog;

    /**
     * Open up the given QFile in the correct platform directory, retaining the given basename.
     */
    bool OpenLocalData (QFile *file, QFile::OpenMode mode);

    /**
     * Activate the given blog.
     *
     * This connects it to the NetworkEngine, sets it as iActiveBlog, signals the UI to display
     * the currently cached data, and then starts refreshing it (using RefreshCurrentBlog).
     */
    void ActivateBlog (WPDataBlog *blog);

    /**
     * Deactivate the currently actie blog
     *
     * This disconnects it from the NetworkEngine, unsets the iActiveBlog.
     */
    void DeactivateBlog ();

friend QDataStream &operator<< (QDataStream &out, const WPDataUsersBlogs &blogs);
friend bool operator>> (QDataStream &in, WPDataUsersBlogs &blogs);
};

QDataStream &operator<< (QDataStream &out, const WPDataUsersBlogs &blogs);
bool operator>> (QDataStream &in, WPDataUsersBlogs &blogs);

/**
 * Base class for persistent per-server-object items such as pages and posts.
 *
 * This class uses the Curiously recurring template pattern to referr to the correct
 * WPDataItemCollection for our subclass.
 *
 * A WPItem can be in several states: Orphaned, Local, Dirty, Normal
 *
 * When a new WPItem is created, it is initially Orphaned. An Orphaned item is not stored in
 * iCollection, and as such, it will have a zero iLocalId.
 *
 * When a newly created WPItem is saved as a local draft, it is assigned an iLocalId and stored
 * in iCollection.
 *
 * When a WPItem contains local modifications that should be stored on the server, it comes Dirty,
 * i.e. iDirty is set. If the WPItem has a proper iItemId, this is an EditPage action; otherwise,
 * it signifies a NewPage action.
 *
 * Finally, if a WPItem is orphaned (i.e. removed from iCollection, and iLocalId set to zero),
 * but it has a valid iItemId, it will be deleted from the server using DeletePage.
 */
template <class DataItemType> class WPDataItem : public QSharedData
{
protected:
    /**
     * A new, empty, orphaned item.
     */
    WPDataItem (WPDataBlog &blog, WPDataItemCollection<DataItemType> &collection);
    virtual ~WPDataItem() {}

/* Generic state */

    /** Parent */
    WPDataBlog &iBlog;

    /** Parent collection */
    WPDataItemCollection<DataItemType> &iCollection;

public: /* XXX: protected */
    /** Server-side ID, or Null string for local item */
    QString iItemId;

    /** Server-side status, or Null string for local item */
    QString iStatus;

    /**
     * Server-side item timestamp for publication, as local time.
     *
     * Defaults to current time for locally created objects.
     */
    QDateTime iDateCreated;

    /** Local ID, or 0 for orphaned item */
    int iLocalId;

    /** Unsaved local changes */
    bool iDirty;

    /** Removing from server */
    bool iDeleted;

    /** Past network request on this failed */
    bool iError;

public:
/* Query state */

    /** Is this item not referenced from the blog's local list of items ? */
    bool IsOrphaned () const
    {
        return iLocalId == 0;
    }

    /** Does this item NOT exist on the server? */
    bool IsLocal () const
    {
        return iItemId.isNull();
    }

    /** Does this item have local changes not yet on the server? */
    bool IsDirty () const
    {
        return iDirty;
    }

    /** The local ID as a string */
    QString LocalId () const
    {
        return QString::number(iLocalId);
    }

    /** 
     * Is this a draft item, that is not publically visible on the server? 
     *
     * XXX: define for WPComment as well (iStatus == "hold")
     */
    bool IsDraft () const
    {
        return IsLocal() || iStatus == "draft" || iStatus == "pending";
    }

    /** Do we have an active network request pending, that affects this item's state? */
    bool IsBusy () const
    {
        // XXX: not so robust
        return (iDirty || iDeleted) && (!iError);
    }

    /** The dateCreated in local time */
    const QDateTime & GetLocalTime () const
    {
        return iDateCreated;
    }
    
    /** The dateCreated in local time as a short string representation */
    QString GetShortLocalTimeString () const
    {
        return iDateCreated.toString("MM/dd HH:mm");
    }

    /** The dateCreated UTC timestamp in the format used for network requests */
    QString GetISO8601Time () const;

/* Change state */
public: /* XXX: protected, called from WPDataItemCollection */
    /** Update our dateCreated timestamp from the given UTC network-format timestamp */
    void SetISO8601Time (const QString &dt);

    /**
     * Assign this item a local ID and store it in the blog's local lists.
     */
    void StoreLocal ();

    /**
     * Set the error flag, indicating that this item's network request has failed, and the changes
     * may not have been saved
     */
    void MarkError ();

public: /* protected XXX: used from WPDataItemCollection */
    /**
     * Set the dirty flag, indicating that this item has local changes that should be synced to the
     * server.
     */
    void SetDirty ();

    /**
     * Item was deleted on server by user.
     */
    virtual void ItemDeleted ()
    {

    }

    /**
     * De-assign this item's local ID, and remove it from the blog's local lists, orphaning this page.
     *
     * Opposite of StoreLocal().
     */
    void DropLocal ();

public:    
    /**
     * Update status to store this page locally, and emit signals for UI update
     */
    void UpdateLocal ();
   
   /**
    * Mark page as having local modifications to synch to server with given server item status, and
    * perform the request
    */ 
    void UpdateStatus (const QString &status, const QList<WPMediaFile> &addedFiles = QList<WPMediaFile>());

    /**
     * Mark this item for deletion, either local or server, as required.
     */
    void Delete ();

public: /* XXX: protected? */
/* Data <-> Network */
    /**
     * Format item data as a WPXmlItem for sending to the server in new/edit request
     */
    virtual void GetContent (WPXmlItem *xml) const = 0;

    /**
     * Update data fields from given server XML-RPC response struct data.
     *
     * This marks the item as not-dirty.
     */
    virtual void SetContent (const WPXmlItem &xml) = 0;

    /**
     * Add a short bit of HTML to the content to link to a media object.
     *
     * This marks the item as dirty.
     */
    virtual void AddSnippet (const QString &html, bool placeOnTop = false) = 0;
};

/*
 * XXX: Qt can't handle signals/slots defined in a template class, but this lets us work around that,
 *      since the signals/slots prototypes don't need template args...
 */
class WPDataItemCollectionInterface : public QObject
{
    Q_OBJECT

signals: /* -> WPNetworkEngine */

    /** Fetch full data for items */
    // XXX: should be GetItems(), but conflicts with iterator below
    void FetchItems ();

    /** Fetch data for induvidual item */
    void GetItem (int localId, QString itemId);

    /** Emitted when a local item first becomes dirty */
    void NewItem (int localId, WPXmlItem item);

    /** Emitted when a normal item becomes dirty */
    void EditItem (int localId, QString itemId, WPXmlItem item);
    
    /** Emitted when a normal item is removed */
    void DeleteItem (int localId, QString itemId);

    /** Upload a file for this item */
    void UploadFiles (int localId, QList<WPMediaFile> files);

protected slots: /* WPNetworkEngine -> */
    virtual void ItemsListed (QList<WPXmlItem> itemList) = 0;
    virtual void ItemGotten (int localId, WPXmlItem item) = 0;
    virtual void ItemCreated (int localId, QString itemId) = 0;
    virtual void ItemEdited (int localId) = 0;
    virtual void ItemDeleted (int localId) = 0;
    
    virtual void ItemFailed (int localId, TWPNetworkEngineCommand command) = 0;
    virtual void FilesUploaded (int localId, QList<WPMediaFile> files) = 0;

signals: /* -> UI */

    /** Any number of items on the given Blog were updated */
    void UpdateItemDataFull (const WPDataBlog &blog);
    
    /** One item on the given Blog was updated */
    void UpdateItemData (const WPDataBlog &blog);

/* XXX: unfortunately, we can't use the template args here... */
    void _ItemAdded (int localId);
    void _ItemChanged (int localId);
    void _ItemRemoved (int localId);
};

/* Forward-declare external template functions */
template <class DataItemType> QDataStream & operator<< (QDataStream &out, const WPDataItemCollection<DataItemType> &collection);
template <class DataItemType> QDataStream & operator>> (QDataStream &in, WPDataItemCollection<DataItemType> &collection);

template <class DataItemType> class WPDataItemCollection : public WPDataItemCollectionInterface
{
public:
    /** Safe pointer type to item object */
    typedef QExplicitlySharedDataPointer<DataItemType> ItemType;

    WPDataItemCollection (WPDataBlog &blog);

private:
    /** Parent blog, used for AssignLocalId */
    WPDataBlog &iBlog;

    /**
     * Locally stored items
     */
    QList<ItemType> iItems;

public:
    /** Iterate over items */
    QListIterator<ItemType> GetItems () const;

    /** Look up item by server id */
    ItemType GetByItemId (const QString &itemId);

    /** Look up item by local id */
    ItemType GetByLocalId (int localId);

    /**
     * Look up a WPPost by local id in string form (argh)
     */
    ItemType GetByLocalId (const QString &localId);

public:
    /** Refresh the item list by requestin the full list of items, and applying it */
    void RefreshAll ();

    /** Called by WPDataItem when state was updated, emits the appropriate signal */
    void UpdateItem (DataItemType &item, QList<WPMediaFile> addedFiles = QList<WPMediaFile>());

protected:    
    /** Full item list updated */
    void ItemsListed (QList<WPXmlItem> itemList);
    
    /** Induvidual item refreshed */
    void ItemGotten (int localId, WPXmlItem item);

    /** Item was created with given ID */
    void ItemCreated (int localId, QString itemId);

    /** Post was saved */
    void ItemEdited (int localId);

    /** Post was deleted */
    void ItemDeleted (int localId);
    
    /** Action on item failed */
    void ItemFailed (int localId, TWPNetworkEngineCommand command);
    
    /** Files for item were uploaded, test if our item, apply URLs and update page if so */
    void FilesUploaded (int localId, QList<WPMediaFile> files);

// let WPDataItem modify iItems and call protected functions
friend class WPDataItem<DataItemType>;

// yay little fishies
friend QDataStream & operator>><> (QDataStream &in, WPDataItemCollection<DataItemType> &collection);

// and this is a backwards fish
friend QDataStream & operator<<<> (QDataStream &out, const WPDataItemCollection<DataItemType> &collection);

};

/**
 * Something. Yay.
 */
class WPDataStats {
public:
    QMap<QDate, int> iVisits;
    QMap<QDate, QList< QPair<QString, int> > > iSearchTerms;
    QMap<QDate, QList< QPair<QString, int> > > iReferrers;
    QDate iLastUpdated;
    int iTotalVisits;
    int iDataStatsVersionNumber;
};

QDataStream &operator<<(QDataStream &out, const WPDataStats &dataStats );
QDataStream &operator>>(QDataStream &in, WPDataStats &dataStats );

/**
 * The central data model object.
 *
 * Represents a single given blog at some XMLRPC URL, with its collection of Pages/Posts/etc.
 *
 * Only one WPDataBlog is active at a given time, and only the active DataBlog may use the
 * WPNetworkManager.
 */
class WPDataBlog : public QObject
{
    Q_OBJECT

private:
    int iLocalIdPool;

    /** The master list of blogs we are part of */
    WPDataUsersBlogs &blogs;

public:
    /**
     * Construct new blog with endpoint values initialized to given values.
     */
    WPDataBlog (WPDataUsersBlogs &blogs, const WPNetworkEndpoint &endpoint = WPNetworkEndpoint());

public:
    /** Our local ID */
    inline QString LocalId () const
    {
        return iEndpoint.iRpcUrl + iBlogId + iEndpoint.iUsername;
    }

public:
    /**
     * Return an per-blog unique ID
     */
    int AssignLocalId ();
 
    /**
     * Iterate over our list of available page items
     */
    QListIterator<WPPage> GetPages () const;

    /**
     * Iterate over our list of available post items
     */
    QListIterator<WPPost> GetPosts () const;

    /**
     * Iterate over our list of available comment items
     */
    QListIterator<WPComment> GetComments () const;

    bool isWordpressHosted() const;

    /**
     * Do we have a persistent API key configured?
     *
     * XXX: if we're still waiting between SetApiKey() and GetStatsSucceeded(), this will return false.
     */
    bool HasApiKey () const;

    /** Do we have some existing stats data avilable? */
    bool HasStatsData () const;

    /**
     * Refresh all pages/posts/comments and aux. data.
     *
     * Also refreshes stats data, if we have an API key configured
     */
    void RefreshAll ();

    /**
     * Refresh media library
     */
    void RefreshMediaLibrary ();

    /**
     * Refresh stats data
     */
    void RefreshStats ();

    /**
     * Add a new category, eventually emitting CategoryAdded
     */
    void AddCategory (QString name);

signals: /* -> Network */
    void GetCategories ();
    void GetMediaLibrary (); //New
    void GetTags ();
    void GetPageStatuses ();
    void GetPostStatuses ();
    void GetCommentStatuses ();
    void GetMediaSettings ();
    void GetStats (QString table, int days);

    /** Create new category, resulting in CategoryCreated with categoryId */
    void NewCategory (int localId, WPXmlItem xml);

    /** Create new media, resulting in MediaCreated with mediaId */
    void NewMedia (int localId, WPXmlItem xml);
    
    /** Signal to WPDataUsersBlogs to update endpoint and refresh the active blog's stats */
    void ApiKeyChanged (WPDataBlog *blog, QString apiKey);

public slots: /* Network -> */
    /**
     * Update iCategories list from category list.
     *
     * The WPDataCategory's are added to the iCategories list such, that a given item's parent
     * is guranteed to be present in the list BEFORE the (child) item; so that the list can be
     * linearly iterated through in a top-down order.
     */
    void CategoriesListed (QList<WPXmlItem> categoryList);

    /** Given new category was created */
    void CategoryCreated (int localId, QString itemId);

    void MediaListed (QList<WPXmlItem> mediaList); //New
    //void MediaCreated (int localId, QString itemId);

    /** API key is valid */
    void GetStatsSucceeded (QString blogUrl, QString apiKey);

    /**
     * Set the statistics API key to use for this blog.
     *
     * If the given api key is the same as the current one, nothing happens.
     *
     * Otherwise, we tell the NetworkEngine to refresh the stats using the new API key, and if that
     * refresh succeeds, we store the new API key in our iEndpoint, which is then then used again
     * next time.
     */
    bool SetApiKey (const QString &apiKey);

signals: /* -> UI */

    void PageAdded (WPPage page);
    void PageChanged (WPPage page);
    void PageRemoved (WPPage page);

    void PostAdded (WPPost post);
    void PostChanged (WPPost post);
    void PostRemoved (WPPost post);

    void CommentAdded (WPComment comment);
    void CommentChanged (WPComment comment);
    void CommentRemoved (WPComment comment);

    void UpdateCategoryData (const WPDataBlog &blog);
    void CategoryAdded (WPDataCategory *category);

    void UpdateMediaData (const WPDataBlog &blog);
    void MediaAdded (WPDataMediaItem *mediaItem);


public slots: /* Collection -> */ 

    void _PageAdded (int localId);
    void _PageChanged (int localId);
    void _PageRemoved (int localId);

    void _PostAdded (int localId);
    void _PostChanged (int localId);
    void _PostRemoved (int localId);

    void _CommentAdded (int localId);
    void _CommentChanged (int localId);
    void _CommentRemoved (int localId);

public: /* XXX: private */
    WPDataItemCollection<WPDataPage> iPages;
    WPDataItemCollection<WPDataPost> iPosts;
    WPDataItemCollection<WPDataComment> iComments;

    /** Top-down sorted list of categories */
    QList<WPDataCategory> iCategories;

    /** XXX: pending new categories */
    QMap<int, WPDataCategory> iNewCategories;

    /** Top-down sorted list of categories */
    QList<WPDataMediaItem> iMediaLibrary;

    // XXX: never used!
    QList<WPDataTag> iTags;

    WPDataStats iStats;

    QMap<QString, QString> iPageStatuses;
    QMap<QString, QString> iPostStatuses;
    QMap<QString, QString> iCommentStatuses;

    QSize iThumbnailSize;
    QSize iMediumSize;
    QSize iLargeSize;

/* Settings */
    /** Our network endpoint, as used by the WPNetworkEngine */
    WPNetworkEndpoint iEndpoint;

    /* Additional info */
    QString iBlogName;
    QString iBlogId;
    QString iIsAdmin;
};

QDataStream &operator<<(QDataStream &out, const WPDataBlog &blog );
QDataStream &operator>>(QDataStream &in, WPDataBlog &blog );







/*
 * WPDataItem(Collection) template method definitions MUST be available
 */
#include "WPDataItem.cpp"







/* Concrete item types */
class WPDataPage : public WPDataItem<WPDataPage>
{
public:
    /* Name of itemId in XML struct */
    static const QString ITEM_ID_KEY;

public:
    /**
     * A new, empty, orphaned page.
     */
    WPDataPage (WPDataBlog &blog);

    ~WPDataPage();

public:
    void GetContent (WPXmlItem *xml) const;
    void SetContent (const WPXmlItem &xml);
    void AddSnippet (const QString &html, bool placeOnTop = false);

    /**
     * Store changes to this Page locally in our WPDataBlog.
     *
     * This will update any UI state for this Page, but will not process it on the server.
     */
    void UpdateLocal (QString title, QString description, QString password, QDateTime dateCreated);

    /**
     * Update changes to this Page to the server, with the given status.
     */
    void UpdateStatus (QString title, QString description, QString password, QDateTime dateCreated, QString status, QList<WPMediaFile> addedFiles);

/* Attributes */
public:
    QString iDescription;
    QString iTitle;
    QString iLink;
    QString iPermaLink;
    QString iExcerpt;
    QString iTextMore;
    QString iWpSlug;
    QString iWpPassword;
    QString iWpAuthor;
    QString iWpPageParentTitle;
    QString iWpAuthorDisplayName;
    QString iWpPageTemplate;
    QString iUserId;
    QString iAllowComments;
    QString iAllowPings;
    QString iPageParentId;
    QString iWpPageOrder;
    QString iWpAuthorId;


    // TODO: Add support for Categories
    // TODO: Add support for custom fields

friend QDataStream &operator<< (QDataStream &, const WPDataPage &);
friend QDataStream &operator>> (QDataStream &, WPDataPage &);
};

QDataStream &operator<<(QDataStream &out, const WPDataPage &page);
QDataStream &operator>>(QDataStream &in, WPDataPage &page);

class WPDataPost : public WPDataItem<WPDataPost>
{
public:
    static const QString ITEM_ID_KEY;

public:
    /**
     * A new, empty, orphaned page.
     */
    WPDataPost (WPDataBlog &blog);

    ~WPDataPost();

public:
    void GetContent (WPXmlItem *xml) const;
    void SetContent (const WPXmlItem &xml);
    void AddSnippet (const QString &html, bool placeOnTop = false);
    
    /**
     * Post was removed, so refresh the list of comments to flush out ones pretaining to
     * this post.
     */
    void ItemDeleted ();

public:
    /**
     * Get the HTML to be used for rendering the displayed post.
     *
     * This is a very simplified version of the transformations that WordPress itself does,
     * so the result will never be a 100% match.
     *
     * WordPress is also customizeable, so any customizations affecting e.g. the 'the_content'
     * hook will break this.
     */
    QString GetDisplayHTML ();

    /**
     * Return a list of Comments for this post
     */
    QList<WPComment> GetComments (); 

    /**
     * Create a new comment for this post, and save it
     */
    WPComment NewComment (const QString &body);

    /**
     * Store changes to this Page locally in our WPDataBlog.
     *
     * This will update any UI state for this Page, but will not process it on the server.
     */
    void UpdateLocal (
            QString title, QString description, QString tags, QDateTime dateCreated,
            QString password, QStringList categories
#if defined(HAVE_QTM_LOCATION)
            , const QtMobility::QGeoPositionInfo& location
#endif // defined(HAVE_QTM_LOCATION)
    );

    /**
     * Update changes to this Page to the server, with the given status.
     */
    void UpdateStatus (
            QString title, QString description, QString tags, QDateTime dateCreated,
            QString password, QStringList categories,
            QString status, QList<WPMediaFile> addedImages
#if defined(HAVE_QTM_LOCATION)
            , const QtMobility::QGeoPositionInfo& location
#endif // defined(HAVE_QTM_LOCATION)
    );

public:
    QString iDescription;
    QString iTitle;
    QString iLink;
    QString iPermaLink;
    QString iUserId;
    QString iMtExcerpt;
    QString iMtTextMore;
    QString iMtAllowComments;
    QString iMtAllowPings;
    QString iMtKeywords;
    QString iWpSlug;
    QString iWpPassword;
    QString iWpAuthorId;
    QString iWpAuthorDisplayName;
    QStringList iCategories;
#if defined(HAVE_QTM_LOCATION)
    QtMobility::QGeoPositionInfo iLocation;
#endif // defined(HAVE_QTM_LOCATION)
    QList<WPDataCustomField> iCustomFields;

friend QDataStream &operator<< (QDataStream &, const WPDataPost &);
friend QDataStream &operator>> (QDataStream &, WPDataPost &);

private:
    //gets the index of a custom field within iCustomFields
    int customFieldIndex(const QString& key) const;

    //gets a custom field object based on a key
    const WPDataCustomField& getCustomField(const QString& key) const;
    
    //checks whether iCustomFields holds all the required fields for location support
    bool locationCustomFieldsExist() const;
};

QDataStream &operator<<(QDataStream &out, const WPDataPost &dataPost );
QDataStream &operator>>(QDataStream &in, WPDataPost &dataPost );

class WPDataComment : public WPDataItem<WPDataComment>
{
public:
    /* Name of itemId in XML struct */
    static const QString ITEM_ID_KEY;

public:
    /**
     * A new, empty, orphaned comment.
     */
    WPDataComment (WPDataBlog &blog);

    /**
     * Create a new comment, and save it
     */
    WPDataComment (WPDataBlog &blog, WPPost post, const QString &body);

    void GetContent (WPXmlItem *xml) const;
    void SetContent (const WPXmlItem &xml);
    void AddSnippet (const QString &html, bool placeOnTop = false);   // XXX: not implemented

public:
/* Status */
    bool IsUnapproved() const;

public:
    QString iUserId;
    QString iParent;
    QString iContent;
    QString iLink;
    QString iPostId;
    QString iPostTitle;
    QString iAuthor;
    QString iAuthorUrl;
    QString iAuthorEmail;
    QString iAuthorIp;
    QString iType;
   
    // XXX: unused 
    bool operator== (const WPDataComment& aOther) const;
    bool operator!= (const WPDataComment& aOther) const { return !operator==(aOther); };
    bool operator< (const WPDataComment& aOther) const;
    bool operator> (const WPDataComment& aOther) const { return aOther < *this; };
};

QDataStream &operator<< (QDataStream &out, const WPDataComment &comment);
QDataStream &operator>> (QDataStream &in, WPDataComment &comment);

class WPDataCategory
{
public:
    /**
     * Initialize to defaults, or values from given struct
     */
    WPDataCategory (const WPXmlItem *xml = NULL);

    /**
     * Update raw page data from given raw XML page struct.
     */
    void SetContent (const WPXmlItem &xml);

public:
    QString iCategoryId;
    QString iParentId;
    QString iDescription;
    QString iCategoryDescription;
    QString iCategoryName;
    QString iHtmlUrl;
    QString iRssUrl;

    int iDataCategoryVersionNumber;
};

QDataStream &operator<<(QDataStream &out, const WPDataCategory &dataCategory );
QDataStream &operator>>(QDataStream &in, WPDataCategory &dataCategory );

/*New Addition*/

class WPDataMediaItem
{
public:
    /**
     * Initialize to defaults, or values from given struct
     */
    WPDataMediaItem (const WPXmlItem *xml = NULL);

    /**
     * Update raw page data from given raw XML page struct.
     */
    void SetContent (const WPXmlItem &xml);

public:
    QString iParentId;
    QString iTitle;
    QString iDescription;
    QString iCaption;
    QString iMetaData;
    QString iLink;
    QDateTime iDateCreated;
    bool iThumbnail;

    int iDataMediaItemVersionNumber;
};

QDataStream &operator<< (QDataStream &out, const WPDataMediaItem &mediaItem);
QDataStream &operator>> (QDataStream &in, WPDataMediaItem &mediaItem);

/*End of New Edition*/

class WPDataTag
{
public:
    /**
     * Initialize to defaults, or values from given struct
     */
    WPDataTag (const WPXmlItem *xml = NULL);
    
    /**
     * Update raw page data from given raw XML page struct.
     */
    void SetContent (const WPXmlItem &xml);

public:
    QString iTagId;
    QString iName;
    QString iCount;
    QString iSlug;
    QString iHtmlUrl;
    QString iRssUrl;
    int iDataTagVersionNumber;
};
QDataStream &operator<<(QDataStream &out, const WPDataTag &dataTag );
QDataStream &operator>>(QDataStream &in, WPDataTag &dataTag );

class WPDataRsdApi
{
public:
    QString iName;
    QString iBlogId;
    QString iPreferred;
    QString iApiLink;
};

#endif /* WPXMLPARSER_H_ */
