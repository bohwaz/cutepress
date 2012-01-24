/*
 * WPData.cpp
 *
 *  Created on: 12.11.2009
 *      Author: thyv
 */

#include "WPData.h"
#include "utils.h"

#include <QDir>
#include <QDesktopServices>
#include <QDebug>

/* WPDataUsersBlogs */
WPDataUsersBlogs::WPDataUsersBlogs (WPNetworkEngine *engine) :
        engine(engine), iActiveBlog(0)
{
    connect(
            this,     SIGNAL(SetEndpoint (WPNetworkEndpoint)),
            engine,     SLOT(SetEndpoint (WPNetworkEndpoint))
    );

    connect(
            this,     SIGNAL(DiscoverEndpoint (WPNetworkEndpoint)),
            engine,     SLOT(DiscoverEndpoint (WPNetworkEndpoint))
    );
    connect(
            this,     SIGNAL(ReportStatsIfNeeded (int)),
            engine,     SLOT(ReportStatsIfNeeded (int))
    );

    connect(
            engine,   SIGNAL(BlogsListed (WPNetworkEndpoint, QList<WPXmlItem>)),
            this,       SLOT(BlogsListed (WPNetworkEndpoint, QList<WPXmlItem>))
    );

}
    
WPDataUsersBlogs::~WPDataUsersBlogs ()
{
    // release blogs
    // XXX: we could just use QSharedData for these as well, but...
    foreach (WPDataBlog *blog, iBlogs)
        delete blog;

    iBlogs.clear();
}

bool WPDataUsersBlogs::LoadLocalData ()
{
    // desired basename
    QFile file("data.txt");

    // open from correct dir
    if (!Utils::openLocalDataFile(file, QIODevice::ReadOnly))
        return false;

    // unserialize our data
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_6);

    // read it in
    // XXX: returning bool from operator>>?
    if (!(in >> *this)) {
        // unable to load data due to version error
        emit DataError("Unable to load local settings: Unknown data version");

        return false;
    }

    // test for data stream errors
    if (in.status() != QDataStream::Ok) {
        // corrupted data
        emit DataError("Unable to load local settings: Corrupted data");

        return false;
    }

    if (iBlogs.isEmpty()){
        // no blogs were saved, so act as if the file wasn't found
        qDebug()<<"No Bloogs found";
        return false;
    }
    else{
        qDebug()<<iBlogs.count()<<"Bloogs found";
        emit UpdateBlogs(*this);
    }

    // will be empty if no active blog
    Q_ASSERT(iActiveBlog);

    // open the blog
    ActivateBlog(iActiveBlog);

    // existing blog loaded
    return true;
}

void WPDataUsersBlogs::SaveLocalData ()
{
    // desired basename
    QFile file("data.txt");

    // open from correct dir
    if (!Utils::openLocalDataFile(file, QIODevice::WriteOnly))
        return;

    // serialize our data
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_6);

    out << *this;
}

WPDataBlog* WPDataUsersBlogs::GetBlog (const QString &localId)
{
    foreach (WPDataBlog *blog, iBlogs) {
        if (blog->LocalId() == localId)
            // found
            return blog;
    }

    // not found
    return NULL;
}

WPDataBlog* WPDataUsersBlogs::GetCurrentBlog ()
{ 
    return iActiveBlog;
}

void WPDataUsersBlogs::OpenBlog (WPDataBlog *newBlog)
{
    //qDebug()<<"WPData.cpp OpenBlog"<<newBlog->iBlogName;
    if (iActiveBlog == newBlog){
        qDebug()<<newBlog->iBlogName<<"already active";
        // already active
        return;
    }

    // switch blogs
    if (iActiveBlog)
        DeactivateBlog();

    if (newBlog)
        ActivateBlog(newBlog);    
}

/**
 * Remove the given blog from our list of blogs
 */
bool WPDataUsersBlogs::DeleteBlog (WPDataBlog *blog)
{
    int index = iBlogs.indexOf(blog);

    Q_ASSERT(blog);
    Q_ASSERT(index >= 0);

    // remove from list
    iBlogs.removeAt(index);

    // update lists
    emit UpdateBlogs(*this);

    // deactive blog?
    if (blog == iActiveBlog) {
        // deactivate
        DeactivateBlog();
    }
    
    // active blog?
    if(!iBlogs.empty()) {
        // pick the next blog (list was shifted forward)
        if (index >= iBlogs.size())
            index = 0;

        WPDataBlog *newBlog = iBlogs[index];

        // activate the new blog
        Q_ASSERT(newBlog);
        ActivateBlog(newBlog);
    }

    // then delete the WPDataBlog
    // XXX: references to it from elsewhere ?!
    delete blog;

    // ok
    return true;
}

void WPDataUsersBlogs::EditBlog (WPDataBlog *blog, const WPNetworkEndpoint &endpoint)
{
    // store
    Q_ASSERT(blog->iEndpoint.iBlogUrl == endpoint.iBlogUrl);
    blog->iEndpoint.iUsername = endpoint.iUsername;
    blog->iEndpoint.iPassword = endpoint.iPassword;
    blog->iEndpoint.iNumberOfPosts = endpoint.iNumberOfPosts;
    blog->iEndpoint.iResizePhotos = endpoint.iResizePhotos;
    blog->iEndpoint.iApiKey = endpoint.iApiKey;
    
    // update engine live if active
    if (iActiveBlog == blog) {
        emit SetEndpoint(blog->iEndpoint);

        // reload with applied settings
        blog->RefreshAll();
    }

    // update lists
    emit UpdateBlogs(*this);
}

void WPDataUsersBlogs::NewBlog (const WPNetworkEndpoint &endpoint)
{
    // force-deactivate current blog
    // we can't issue any requests on the NetworkEngine while we're discovering an incomplete endpoint.
    if (iActiveBlog)
        DeactivateBlog();

    // attempt to find the XML-RPC URL and blog ID, waiting for the BlogsListed result
    emit DiscoverEndpoint(endpoint);
}
    
void WPDataUsersBlogs::RefreshCurrentBlog ()
{
    qDebug()<<"Refreshing Current Bloog";
    // refresh items + misc
    if (iActiveBlog)
        iActiveBlog->RefreshAll();

    if (!iBlogs.isEmpty())
        // check if we need to report stats again
        emit ReportStatsIfNeeded (GetBlogCount());
}
    
void WPDataUsersBlogs::BlogsListed (WPNetworkEndpoint endpoint, QList<WPXmlItem> blogList)
{
    // previous blog count
    int oldBlogs = iBlogs.size();

    // go through the blog list, adding each NEW blog, using info both from the discovery endpoint
    // used, as well as the XMLRPC response.
    for (QListIterator<WPXmlItem> i(blogList); i.hasNext(); ) {
        const WPXmlItem &xml = i.next();
        bool found = false;

        // ignore blog entries that we already have
        foreach (WPDataBlog *blog, iBlogs) {
            if (
                    (blog->iEndpoint.iRpcUrl == xml.content["xmlrpc"])
                &&  (blog->iEndpoint.iBlogId == xml.content["blogid"])
                &&  (blog->iEndpoint.iUsername == endpoint.iUsername)
            ) {
                // duplicate
                found = true;
                break;
            }
        }

        if (!found) {
            // new blog, using base vars from endpoint
            WPDataBlog *blog = new WPDataBlog(*this, endpoint);

            // update additional fields in the new blog's endpoint
            blog->iEndpoint.iBlogUrl = xml.content["url"];
            blog->iEndpoint.iRpcUrl = xml.content["xmlrpc"];
            blog->iEndpoint.iBlogId = xml.content["blogid"];

            // update additional info
            blog->iBlogName = xml.content["blogName"];
            blog->iIsAdmin = xml.content["isAdmin"];

            // keep
            iBlogs.append(blog);
        }
    }

    // this is ONLY called after NewBlog, and NewBlog will always deactivate the current blog
    Q_ASSERT(!iActiveBlog);

    // Try to find the blog the user wanted to activate from the new blogs
    for(int i = oldBlogs; i < iBlogs.count(); ++i) {
        if(iBlogs[i]->iEndpoint.iBlogUrl.contains(endpoint.iBlogUrl, Qt::CaseInsensitive)) {
            ActivateBlog(iBlogs[i]);
            break;
        }
    }
    
    // If the above failed, fall back to something else
    if(!iActiveBlog) {
        // pick the first newly added blog as active
        if (iBlogs.size() > oldBlogs)
            ActivateBlog(iBlogs[oldBlogs]);
        else if (iBlogs.size() > 0)
            // XXX: no new blogs, which one now?
            ActivateBlog(iBlogs[0]);
    }

    // blog discovery was succesfull, UI can continue
    emit GetUserBlogsCompleted(*iActiveBlog);
    
    // update lists
    emit UpdateBlogs(*this);
}
    
void WPDataUsersBlogs::BlogApiKeyUpdated (WPDataBlog *blog, QString apiKey)
{
    if (blog != iActiveBlog)
        return;

    WPNetworkEndpoint endpoint (blog->iEndpoint);

    // construct temporary endpoint with new api key
    endpoint.iApiKey = apiKey;

    // update engine to temporarily use this api key
    //emit SetEndpoint(endpoint);

    // reload stats using new api key
    // if this succeeds, the new api key is stored in our iendpoint
    //blog->RefreshStats();
}

void WPDataUsersBlogs::ActivateBlog (WPDataBlog *blog)
{
    Q_ASSERT(blog);
    // XXX: Q_ASSERT(!iActiveBlog || iActiveBlog == blog);

    // set
    iActiveBlog = blog;

    // tell the engine to use it
    // XXX: legacy
    engine->iBlogData = blog;

    // XXX: threadsafe with the engine....
    emit SetEndpoint(blog->iEndpoint);

/* blog -> engine */
    // refresh
    connect(
      &blog->iPages, SIGNAL(FetchItems ()),
        engine,        SLOT(GetPages ())
    );
    connect(
      &blog->iPosts, SIGNAL(FetchItems ()),
        engine,        SLOT(GetPosts ())
    );
    connect(
      &blog->iComments, SIGNAL(FetchItems ()),
        engine,           SLOT(GetComments ())
    );
    connect(
        blog,       SIGNAL(GetCategories ()),
        engine,       SLOT(GetCategories ())
    );
    connect(
        blog,       SIGNAL(GetMediaLibrary()),
        engine,       SLOT(GetMedia())
    );

    // get
    connect(
      &blog->iPages, SIGNAL(GetItem (int, QString)),
        engine,        SLOT(GetPage (int, QString))
    );
    connect(
      &blog->iPosts, SIGNAL(GetItem (int, QString)),
        engine,        SLOT(GetPost (int, QString))
    );
    connect(
      &blog->iComments, SIGNAL(GetItem (int, QString)),
        engine,           SLOT(GetComment (int, QString))
    );

    // new
    connect(
      &blog->iPages, SIGNAL(NewItem (int, WPXmlItem)),
        engine,        SLOT(NewPage (int, WPXmlItem))
    );
    connect(
      &blog->iPosts, SIGNAL(NewItem (int, WPXmlItem)),
        engine,        SLOT(NewPost (int, WPXmlItem))
    );
    connect(
      &blog->iComments, SIGNAL(NewItem (int, WPXmlItem)),
        engine,           SLOT(NewComment (int, WPXmlItem))
    );
    connect(
        blog,           SIGNAL(NewCategory (int, WPXmlItem)),
        engine,           SLOT(NewCategory (int, WPXmlItem))
    );

    // edit
    connect(
      &blog->iPages, SIGNAL(EditItem (int, QString, WPXmlItem)),
        engine,        SLOT(EditPage (int, QString, WPXmlItem))
    );
    connect(
      &blog->iPosts, SIGNAL(EditItem (int, QString, WPXmlItem)),
        engine,        SLOT(EditPost (int, QString, WPXmlItem))
    );
    connect(
      &blog->iComments, SIGNAL(EditItem (int, QString, WPXmlItem)),
        engine,           SLOT(EditComment (int, QString, WPXmlItem))
    );

    // delete
    connect(
      &blog->iPages, SIGNAL(DeleteItem (int, QString)),
        engine,        SLOT(DeletePage (int, QString))
    );
    connect(
      &blog->iPosts, SIGNAL(DeleteItem (int, QString)),
        engine,        SLOT(DeletePost (int, QString))
    );
    connect(
      &blog->iComments, SIGNAL(DeleteItem (int, QString)),
        engine,           SLOT(DeleteComment (int, QString))
    );

    // upload
    connect(
      &blog->iPages, SIGNAL(UploadFiles (int, QList<WPMediaFile>)),
        engine,        SLOT(UploadFiles (int, QList<WPMediaFile>))
    );
    connect(
      &blog->iPosts, SIGNAL(UploadFiles (int, QList<WPMediaFile>)),
        engine,        SLOT(UploadFiles (int, QList<WPMediaFile>))
    );
    // no uploads for comments

    // misc
    connect(
        blog,       SIGNAL(GetTags ()),
        engine,       SLOT(GetTags ())
    );
    connect(
        blog,       SIGNAL(GetPageStatuses ()),
        engine,       SLOT(GetPageStatuses ())
    );
    connect(
        blog,       SIGNAL(GetPostStatuses ()),
        engine,       SLOT(GetPostStatuses ())
    );
    connect(
        blog,       SIGNAL(GetCommentStatuses ()),
        engine,       SLOT(GetCommentStatuses ())
    );
    connect(
        blog,       SIGNAL(GetMediaSettings ()),
        engine,       SLOT(GetMediaSettings ())
    );
    connect(
        blog,       SIGNAL(GetStats (QString, int)),
        engine,       SLOT(GetStats (QString, int))
    );

/* blog -> this */
    // stats
    connect(
        blog,       SIGNAL(ApiKeyChanged(WPDataBlog *, QString)),
        this,         SLOT(BlogApiKeyUpdated(WPDataBlog *, QString))
    );

/* engine -> blog */
    // listed
    connect(
        engine,    SIGNAL(PagesListed (QList<WPXmlItem>)),
      &blog->iPages, SLOT(ItemsListed (QList<WPXmlItem>))
    );
    connect(
        engine,    SIGNAL(PostsListed (QList<WPXmlItem>)),
      &blog->iPosts, SLOT(ItemsListed (QList<WPXmlItem>))
    );
    connect(
        engine,       SIGNAL(CommentsListed (QList<WPXmlItem>)),
      &blog->iComments, SLOT(ItemsListed (QList<WPXmlItem>))
    );
    connect(
        engine,       SIGNAL(CategoriesListed (QList<WPXmlItem>)),
        blog,           SLOT(CategoriesListed (QList<WPXmlItem>))
    );
    connect(
        engine,       SIGNAL(MediaListed(QList<WPXmlItem>)),
        blog,           SLOT(MediaListed(QList<WPXmlItem>))
    );
    connect(
        engine,       SIGNAL(ApiKeyReceived(QString)),
        blog,           SLOT(SetApiKey(QString))
    );

    // gotten
    connect(
        engine,    SIGNAL(PageGotten (int, WPXmlItem)),
      &blog->iPages, SLOT(ItemGotten (int, WPXmlItem))
    );
    connect(
        engine,    SIGNAL(PostGotten (int, WPXmlItem)),
      &blog->iPosts, SLOT(ItemGotten (int, WPXmlItem))
    );
    connect(
        engine,       SIGNAL(CommentGotten (int, WPXmlItem)),
      &blog->iComments, SLOT(ItemGotten (int, WPXmlItem))
    );

    // created
    connect(
        engine,    SIGNAL(PageCreated (int, QString)),
      &blog->iPages, SLOT(ItemCreated (int, QString))
    );
    connect(
        engine,    SIGNAL(PostCreated (int, QString)),
      &blog->iPosts, SLOT(ItemCreated (int, QString))
    );
    connect(
        engine,       SIGNAL(CommentCreated (int, QString)),
      &blog->iComments, SLOT(ItemCreated (int, QString))
    );
    connect(
        engine,       SIGNAL(CategoryCreated (int, QString)),
        blog,           SLOT(CategoryCreated (int, QString))
    );

    // edited
    connect(
        engine,    SIGNAL(PageEdited (int)),
      &blog->iPages, SLOT(ItemEdited (int))
    );
    connect(
        engine,    SIGNAL(PostEdited (int)),
      &blog->iPosts, SLOT(ItemEdited (int))
    );
    connect(
        engine,       SIGNAL(CommentEdited (int)),
      &blog->iComments, SLOT(ItemEdited (int))
    );

    // deleted
    connect(
        engine,    SIGNAL(PageDeleted (int)),
      &blog->iPages, SLOT(ItemDeleted (int))
    );
    connect(
        engine,    SIGNAL(PostDeleted (int)),
      &blog->iPosts, SLOT(ItemDeleted (int))
    );
    connect(
        engine,       SIGNAL(CommentDeleted (int)),
      &blog->iComments, SLOT(ItemDeleted (int))
    );

    // failed
    connect(
        engine,    SIGNAL(PageFailed (int, TWPNetworkEngineCommand)),
      &blog->iPages, SLOT(ItemFailed (int, TWPNetworkEngineCommand))
    );
    connect(
        engine,    SIGNAL(PostFailed (int, TWPNetworkEngineCommand)),
      &blog->iPosts, SLOT(ItemFailed (int, TWPNetworkEngineCommand))
    );
    connect(
        engine,       SIGNAL(CommentFailed (int, TWPNetworkEngineCommand)),
      &blog->iComments, SLOT(ItemFailed (int, TWPNetworkEngineCommand))
    );

    // files uploaded, connect to both, they can distinguish based on the localId
    connect(
        engine,    SIGNAL(FilesUploaded (int, QList<WPMediaFile>)),
      &blog->iPages, SLOT(FilesUploaded (int, QList<WPMediaFile>))
    );
    connect(
        engine,    SIGNAL(FilesUploaded (int, QList<WPMediaFile>)),
      &blog->iPosts, SLOT(FilesUploaded (int, QList<WPMediaFile>))
    );

    // stats
    connect(
        engine,    SIGNAL(GetStatsSucceeded (QString, QString)),
        blog,        SLOT(GetStatsSucceeded (QString, QString))
    );

/* blog -> ui, as signal -> signal connections */
    connect(
      &blog->iPages, SIGNAL(UpdateItemDataFull (const WPDataBlog &)),
        this,        SIGNAL(UpdatePageDataFull (const WPDataBlog &))
    );
    connect(
      &blog->iPages, SIGNAL(UpdateItemData (const WPDataBlog &)),
        this,        SIGNAL(UpdatePageData (const WPDataBlog &))
    );

    connect(
      &blog->iPosts, SIGNAL(UpdateItemDataFull (const WPDataBlog &)),
        this,        SIGNAL(UpdatePostDataFull (const WPDataBlog &))
    );
    connect(
      &blog->iPosts, SIGNAL(UpdateItemData (const WPDataBlog &)),
        this,        SIGNAL(UpdatePostData (const WPDataBlog &))
    );
    
    connect(
      &blog->iComments, SIGNAL(UpdateItemDataFull (const WPDataBlog &)),
        this,           SIGNAL(UpdateCommentDataFull (const WPDataBlog &))
    );
    connect(
      &blog->iComments, SIGNAL(UpdateItemData (const WPDataBlog &)),
        this,           SIGNAL(UpdateCommentData (const WPDataBlog &))
    );

    connect(
        blog,           SIGNAL(UpdateCategoryData (const WPDataBlog &)),
        this,           SIGNAL(UpdateCategoryData (const WPDataBlog &))
    );

    connect(
        blog,           SIGNAL(UpdateMediaData (const WPDataBlog &)),
        this,           SIGNAL(UpdateMediaData(const WPDataBlog &))
    );

    // update locally cached lists to UI while refreshing
    emit UpdatePageDataFull(*blog);
    emit UpdatePostDataFull(*blog);
    emit UpdateCommentDataFull(*blog);
    emit UpdateCategoryData(*blog);
    emit UpdateMediaData(*blog);

    /* XXX: update to UI:

        emit UpdateBlogs(*iDataUsersBlogs);
        emit UpdateData(*blog);
        emit UpdateCategoryData(*iBlogData);
        emit UpdateStats(iBlogData->iStats);

   */

    // fetch fresh data from network
    RefreshCurrentBlog();
}

void WPDataUsersBlogs::DeactivateBlog ()
{
    WPDataBlog *blog = iActiveBlog;
    Q_ASSERT(blog);

    // unset
    iActiveBlog = NULL;
    
    // XXX: sneak it out
    // XXX: should use UnsetEndpoint or whatever
    engine->iBlogData = NULL;
    engine->ClearRequestQueue();

    // disconnect everything engine <-> blog
    disconnect(blog, NULL, engine, NULL);
    disconnect(&blog->iPages, NULL, engine, NULL);
    disconnect(&blog->iPosts, NULL, engine, NULL);
    disconnect(&blog->iComments, NULL, engine, NULL);
    disconnect(engine, NULL, blog, NULL);
    disconnect(engine, NULL, &blog->iPages, NULL);
    disconnect(engine, NULL, &blog->iPosts, NULL);
    disconnect(engine, NULL, &blog->iComments, NULL);

    // disconnect blog -> ui signals
    disconnect(&blog->iPages, NULL, this, NULL);
    disconnect(&blog->iPosts, NULL, this, NULL);
    disconnect(&blog->iComments, NULL, this, NULL);

    // disconnect blog -> this 
    disconnect(blog, NULL, this, NULL);

    /* XXX: flush engine? This is currently done by ActivateBlog...
    WPDataBlog nullBlog(*this);
    emit UpdatePageDataFull(nullBlog);
    emit UpdatePostDataFull(nullBlog);
    emit UpdateCommentDataFull(nullBlog);
    emit UpdateCategoryData(nullBlog);
    */
}

/* WPDataBlog */
WPDataBlog::WPDataBlog (WPDataUsersBlogs &blogs, const WPNetworkEndpoint &endpoint) :
    iLocalIdPool(1), 
    blogs(blogs), 
    iPages(*this),
    iPosts(*this),
    iComments(*this),
    iThumbnailSize(QSize(150,150)),
    iMediumSize(QSize(640,480)),
    iLargeSize(QSize(1024,768)),
    iEndpoint(endpoint)
{
    // comments
    connect(
       &iComments,  SIGNAL(_ItemAdded (int)),
        this,         SLOT(_CommentAdded (int))
    );
    connect(
       &iComments,  SIGNAL(_ItemChanged (int)),
        this,         SLOT(_CommentChanged (int))
    );
    connect(
       &iComments,  SIGNAL(_ItemRemoved (int)),
        this,         SLOT(_CommentRemoved (int))
    );

    // posts
    connect(
       &iPosts,  SIGNAL(_ItemAdded (int)),
        this,      SLOT(_PostAdded (int))
    );
    connect(
       &iPosts,  SIGNAL(_ItemChanged (int)),
        this,      SLOT(_PostChanged (int))
    );
    connect(
       &iPosts, SIGNAL(_ItemRemoved (int)),
        this,     SLOT(_PostRemoved (int))
    );

    // pages
    connect(
       &iPages,  SIGNAL(_ItemAdded (int)),
        this,      SLOT(_PageAdded (int))
    );
    connect(
       &iPages,  SIGNAL(_ItemChanged (int)),
        this,      SLOT(_PageChanged (int))
    );
    connect(
       &iPages, SIGNAL(_ItemRemoved (int)),
        this,     SLOT(_PageRemoved (int))
    );

}

int WPDataBlog::AssignLocalId ()
{
    return iLocalIdPool++;
}

QListIterator<WPPage> WPDataBlog::GetPages () const
{
    return iPages.GetItems();
}

QListIterator<WPPost> WPDataBlog::GetPosts () const
{
    return iPosts.GetItems();
}
    
QListIterator<WPComment> WPDataBlog::GetComments () const
{
    return iComments.GetItems();
}
    
bool WPDataBlog::SetApiKey (const QString &apiKey) {
    if (apiKey == iEndpoint.iApiKey)
        // nothing
        return false;

    iEndpoint.iApiKey = apiKey;
    // test it, by temporarily setting it on the networkengine
    //emit ApiKeyChanged(this, apiKey);

    // changed
    return true; 
}
     
void WPDataBlog::GetStatsSucceeded (QString blogUrl, QString apiKey)
{
    if (blogUrl != iEndpoint.iBlogUrl)
        // XXX: spurious, probably switched blogs?
        return;

    // store permanently
    // XXX: this will be run X million times, but does it matter?
    iEndpoint.iApiKey = apiKey;
}

bool WPDataBlog::isWordpressHosted() const
{
    return iEndpoint.isWordpressHosted();
}

bool WPDataBlog::HasApiKey () const
{
    return !iEndpoint.iApiKey.isEmpty();
}
    
bool WPDataBlog::HasStatsData () const
{
    return iStats.iLastUpdated.isValid();
}

void WPDataBlog::RefreshAll ()
{
    // item collections
    iPages.RefreshAll();
    iPosts.RefreshAll();
    iComments.RefreshAll();

    // others
    emit GetCategories();
    if(this->isWordpressHosted())
        emit GetMediaLibrary();
    //emit GetTags(); not used for anything currently
    emit GetPageStatuses();
    emit GetPostStatuses();
    emit GetCommentStatuses();
    emit GetMediaSettings();

    // XXX: always refresh stats as well?
//    if (HasApiKey())
//        RefreshStats();
}

void WPDataBlog::RefreshMediaLibrary()
{
    if(this->isWordpressHosted())
        emit GetMediaLibrary();
}

void WPDataBlog::RefreshStats ()
{
    int days = 366;

    if (!iStats.iLastUpdated.isNull())
        // days since last update
        days = iStats.iLastUpdated.daysTo(QDate::currentDate()) + 1;

    // stats
    emit GetStats("", days);            // custom
    emit GetStats("searchterms", 7);    // week
    emit GetStats("referrers", 7);      // week
}

// XXX: by hand D:
void WPDataBlog::AddCategory (QString name)
{
    WPDataCategory category;

    // build
    category.iCategoryName = name;

    // xml item
    WPXmlItem xml;

    xml.content["name"] = name;

    // track
    int localId = AssignLocalId();
    
    iNewCategories.insert(localId, category);

    // send
    emit NewCategory(localId, xml);
}

void WPDataBlog::CategoriesListed (QList<WPXmlItem> categoryList)
{
    // category IDs that have been added to the list
    // XXX: this use of a QSet as an intermediate step is suboptimal?
    QSet<QString> processedItems;

    // build the sorted list of categories
    iCategories.clear();

    // process items in multiple iterations until we've found all parents
    while (!categoryList.isEmpty()) {
        // avoid potential infinite loops caused by items with a missing parent
        bool found = false;

        // qDebug() << "WPDataBlog::CategoriesListed:" << categoryList.size() << "remaining";

        // find all available children for this set of parents
        for (QMutableListIterator<WPXmlItem> i(categoryList); i.hasNext(); ) {
            const WPXmlItem &xml = i.next();

            const QString &categoryId = xml.content["categoryId"];
            const QString &parentId = xml.content["parentId"];

            // qDebug() << "WPDataBlog::CategoriesListed:" << categoryId << "->" << parentId;
            
            // process items that are top-level, or we already have their parent
            if (parentId.isEmpty() || parentId == "0" || processedItems.contains(parentId)) {
                // new WPDataCategory item
                iCategories.append(WPDataCategory(&xml));

                // qDebug() << "WPDataBlog::CategoriesListed:" << categoryId << "OK";

                // mark as processed
                found = true;
                i.remove();
                processedItems.insert(categoryId);
            }
        }

        if (!found) {
            // XXX: orphaned categories!
            qWarning() << "EGetCategories: Omitting" << categoryList.size() << "categories with missing parents";
            break;
        }
    }

    // update UI
    emit UpdateCategoryData(*this);
}

void WPDataBlog::MediaListed (QList<WPXmlItem> mediaList)
{
    // category IDs that have been added to the list
    // XXX: this use of a QSet as an intermediate step is suboptimal?
    //QSet<QString> processedItems;

    // build the sorted list of categories
    iMediaLibrary.clear();

    // process items in multiple iterations until we've found all parents
    while (!mediaList.isEmpty()) {
        // avoid potential infinite loops caused by items with a missing parent
        bool found = false;

        // qDebug() << "WPDataBlog::CategoriesListed:" << categoryList.size() << "remaining";

        // find all available children for this set of parents
        for (QMutableListIterator<WPXmlItem> i(mediaList); i.hasNext(); ) {
            const WPXmlItem &xml = i.next();

            //const QString &link = xml.content["link"];
            const QString &parentId = xml.content["parent"];


            // process items that are top-level, or we already have their parent
            //if (parentId.isEmpty() || parentId == "0" /*|| processedItems.contains(parentId)*/) {
                // new WPDataMedia item
                iMediaLibrary.append(WPDataMediaItem(&xml));


                // mark as processed
                found = true;
                i.remove();
                //processedItems.insert(link);
            //}
        }

        if (!found) {
            // XXX: orphaned media items!
            qWarning() << "EGetMediaLinrary: Omitting" << mediaList.size() << "media items with missing parents";
            break;
        }
    }

    // update UI
    emit UpdateMediaData(*this);
}
    
void WPDataBlog::CategoryCreated (int localId, QString itemId)
{
    WPDataCategory category = iNewCategories.take(localId);

    // apply
    category.iCategoryId = itemId;

    // store
    // just stick it at the end, it's a top-level category, so it can be anywhere
    iCategories.append(category);

    // update ui
    // XXX: with pointer to real item in list
    emit CategoryAdded(&iCategories.last());
}

/* Incremental update signals */
// Pages
void WPDataBlog::_PageAdded (int localId)
{ 
    WPPage p = iPages.GetByLocalId(localId); 
    
    if (p)
        emit PageAdded(p);
}
   
void WPDataBlog::_PageChanged (int localId)
{
    WPPage p = iPages.GetByLocalId(localId); 
    
    if (p) 
        emit PageChanged(p);
}

    
void WPDataBlog::_PageRemoved (int localId)
{ 
    WPPage p = iPages.GetByLocalId(localId); 
    
    if (p) 
        emit PageRemoved(p);
}

// Posts
void WPDataBlog::_PostAdded (int localId)
{ 
    WPPost p = iPosts.GetByLocalId(localId); 
    
    if (p){
        qDebug()<<"Poost Added WPData.h";
        emit PostAdded(p);
    }
}
   
void WPDataBlog::_PostChanged (int localId)
{
    WPPost p = iPosts.GetByLocalId(localId); 
    
    if (p) 
        emit PostChanged(p);
}

    
void WPDataBlog::_PostRemoved (int localId)
{ 
    WPPost p = iPosts.GetByLocalId(localId); 
    
    if (p) 
        emit PostRemoved(p);
}

// Comments
void WPDataBlog::_CommentAdded (int localId)
{
    WPComment c = iComments.GetByLocalId(localId); 
    
    if (c) {
        qDebug()<<"Coomment Added WPData.h";
        emit CommentAdded(c);
    }
}
   
void WPDataBlog::_CommentChanged (int localId)
{
    WPComment c = iComments.GetByLocalId(localId); 
    
    if (c) 
        emit CommentChanged(c); 
}

    
void WPDataBlog::_CommentRemoved (int localId)
{ 
    WPComment c = iComments.GetByLocalId(localId);

    if (c){
        emit CommentRemoved(c); 
    }
}

/** WPDataCategory */
WPDataCategory::WPDataCategory (const WPXmlItem *xml)
{
    if (xml)
        SetContent(*xml);
}

void WPDataCategory::SetContent (const WPXmlItem &xml)
{
    iCategoryId             = xml.content["categoryId"];
    iParentId               = xml.content["parentId"];
    iDescription            = xml.content["description"];
    iCategoryDescription    = xml.content["categoryDescription"];
    iCategoryName           = xml.content["categoryName"];
    iHtmlUrl                = xml.content["htmlUrl"];
    iRssUrl                 = xml.content["rssUrl"];
}

/** WPDataCategory */
WPDataMediaItem::WPDataMediaItem (const WPXmlItem *xml)
{
    if (xml)
        SetContent(*xml);
}

void WPDataMediaItem::SetContent (const WPXmlItem &xml)
{
    iTitle                  = xml.content["title"];
    iParentId               = xml.content["parentId"];
    iDescription            = xml.content["description"];
    iThumbnail              = xml.content["thumbnail"]=="0"?true:false;
    iCaption                = xml.content["caption"];
    iLink                   = xml.content["link"];
    QString ext = xml.content["link"];
    iMetaData               = xml.content["metadata"];
    iDateCreated            = QDateTime::fromString(xml.content["date_created_gmt"], "yyyyMMddTHH:mm:ss");
    iDateCreated.setTimeSpec(Qt::UTC);
}

/** WPDataTag */
WPDataTag::WPDataTag (const WPXmlItem *xml)
{
    if (xml)
        SetContent(*xml);
}

void WPDataTag::SetContent (const WPXmlItem &xml)
{
    iTagId                  = xml.content["tag_id"];
    iName                   = xml.content["name"];
    iCount                  = xml.content["count"];
    iSlug                   = xml.content["slug"];
    iHtmlUrl                = xml.content["html_url"];
    iRssUrl                 = xml.content["rss_url"];
}

/* Serialization*/
QDataStream &operator<< (QDataStream &out, const WPDataUsersBlogs &blogs)
{
    out << KDataUsersBlogsVersionNumber;
    out << (quint32) blogs.iBlogs.size();
   
    foreach (const WPDataBlog *blog, blogs.iBlogs) {
        out << *blog;
    }

    // iActiveBlogIndex
    int index;

    // find index for active blog
    for (index = 0; index < blogs.iBlogs.size(); index++) {
        if (blogs.iActiveBlog == blogs.iBlogs[index])
            break;
    }

    // store, iBlogs.size() if no valid active blog
    out << index;

    return out;
}

bool operator>> (QDataStream &in, WPDataUsersBlogs &blogs )
{
    int version;
    
    in >> version;

    if (version > KDataUsersBlogsVersionNumber)
        // data format too new
        return false;

    quint32 count;

    in >> count;

    while (count--) {
        WPDataBlog *blog = new WPDataBlog(blogs);

        in >> *blog;

        blogs.iBlogs.append(blog);
    }

    // iActiveBlogIndex
    int index;

    in >> index;

    // normalize
    // index will be == size if there was no active blog
    if (index < 0 || index >= blogs.iBlogs.size())
        // first blog
        index = 0;

    // set, unless there aren't any at all
    if (!blogs.iBlogs.isEmpty())
        blogs.iActiveBlog = blogs.iBlogs[index];

    // ok
    return true;
}


QDataStream &operator<< (QDataStream &out, const WPDataBlog &blog )
{
    out 
        << KDataBlogVersionNumber
        << blog.iPages
        << blog.iPosts
        << blog.iCategories
        << blog.iMediaLibrary
        << blog.iTags
        << blog.iComments
        << blog.iPageStatuses
        << blog.iPostStatuses
        << blog.iCommentStatuses
        << blog.iEndpoint.iUsername
        << blog.iEndpoint.iPassword 
        << blog.iEndpoint.iBlogUrl 
        << blog.iEndpoint.iRpcUrl
        << blog.iBlogName
        << blog.iEndpoint.iBlogId
        << blog.iIsAdmin
        << blog.iEndpoint.iNumberOfPosts
        << blog.iEndpoint.iResizePhotos
        << blog.iEndpoint.iApiKey
        << blog.iStats
    ;

    return out;
}

QDataStream &operator>> (QDataStream &in, WPDataBlog &blog )
{
    int version;

    // iDataBlogVersionNumber
    in 
        >> version
        >> blog.iPages
    ;

    if (version < 3) {
        // iLocalDraftPages
        // XXX: is there really any difference here, except the iItemId that's read in?
        in >> blog.iPages;
    }

    in >> blog.iPosts;

    if (version < 4) {
        // iLocalDraftPosts
        // XXX: is there really any difference here, except the iItemId that's read in?
        in >> blog.iPosts;
    };

    in
        >> blog.iCategories
        >> blog.iMediaLibrary
        >> blog.iTags 
        >> blog.iComments
        >> blog.iPageStatuses
        >> blog.iPostStatuses
        >> blog.iCommentStatuses
        >> blog.iEndpoint.iUsername
        >> blog.iEndpoint.iPassword
        >> blog.iEndpoint.iBlogUrl
        >> blog.iEndpoint.iRpcUrl
        >> blog.iBlogName
        >> blog.iEndpoint.iBlogId
        >> blog.iIsAdmin
        >> blog.iEndpoint.iNumberOfPosts
        >> blog.iEndpoint.iResizePhotos
    ;

    if (version >= 2) {
        in >> blog.iEndpoint.iApiKey >> blog.iStats;
    }

    return in;
}

QDataStream &operator<<(QDataStream &out, const WPDataCategory &dataPage )
{
    out << KDataCategoryVersionNumber << dataPage.iCategoryId << dataPage.iParentId << dataPage.iDescription
            << dataPage.iCategoryDescription << dataPage.iCategoryName << dataPage.iHtmlUrl
            << dataPage.iRssUrl;
    return out;
}

QDataStream &operator>>(QDataStream &in, WPDataCategory &dataPage )
{
    in >> dataPage.iDataCategoryVersionNumber >> dataPage.iCategoryId >> dataPage.iParentId >> dataPage.iDescription
            >> dataPage.iCategoryDescription >> dataPage.iCategoryName >> dataPage.iHtmlUrl
            >> dataPage.iRssUrl;
    return in;
}

QDataStream &operator<<(QDataStream &out, const WPDataMediaItem &dataPage )
{
    out << KDataMediaItemVersionNumber << dataPage.iParentId << dataPage.iTitle << dataPage.iDescription
            << dataPage.iCaption << dataPage.iMetaData << dataPage.iLink << dataPage.iDateCreated
            << dataPage.iThumbnail;
    return out;
}

QDataStream &operator>>(QDataStream &in, WPDataMediaItem &dataPage )
{
    in >> dataPage.iDataMediaItemVersionNumber >> dataPage.iParentId >> dataPage.iTitle >> dataPage.iDescription
            >> dataPage.iCaption >> dataPage.iMetaData >> dataPage.iLink >> dataPage.iDateCreated
            >> dataPage.iThumbnail;
    return in;
}

QDataStream &operator<<(QDataStream &out, const WPDataTag &dataTag )
{
    out << KDataTagVersionNumber << dataTag.iTagId << dataTag.iName << dataTag.iCount
            << dataTag.iSlug << dataTag.iHtmlUrl << dataTag.iRssUrl;
    return out;
}

QDataStream &operator>>(QDataStream &in, WPDataTag &dataTag )
{
    in >> dataTag.iDataTagVersionNumber >> dataTag.iTagId >> dataTag.iName >> dataTag.iCount
            >> dataTag.iSlug >> dataTag.iHtmlUrl >> dataTag.iRssUrl;
    return in;
}

QDataStream &operator<<(QDataStream &out, const WPDataStats &dataStats )
{
    out << KDataStatsVersionNumber << dataStats.iTotalVisits << dataStats.iVisits << dataStats.iSearchTerms << dataStats.iReferrers << dataStats.iLastUpdated;
    return out;
}

QDataStream &operator>>(QDataStream &in, WPDataStats &dataStats )
{
    in >> dataStats.iDataStatsVersionNumber >> dataStats.iTotalVisits;

    if (dataStats.iDataStatsVersionNumber == 1) {
        QList< QPair<QDate, int> > visits = QList< QPair<QDate, int> >();
        in >> visits;

        QPair<QDate, int> pair;
        foreach (pair, visits) {
            dataStats.iVisits.insert(pair.first, pair.second);
        }
    } else if (dataStats.iDataStatsVersionNumber >= 2) {
        in >> dataStats.iVisits >> dataStats.iSearchTerms >> dataStats.iReferrers;
    }

    if (dataStats.iDataStatsVersionNumber >= 3) {
        in >> dataStats.iLastUpdated;
    }

    return in;
}
