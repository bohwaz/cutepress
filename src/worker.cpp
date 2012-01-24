/**************************************************************************
 *    CutePress
 *    Copyright (C) 2012 Abhishek Kumar <abhishek.mrt22@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/

#include "worker.h"
#include<QDesktopServices>
#include <QFileInfo>
#include <QApplication>
#include <QUrl>

Worker::Worker()
{
    qDebug()<<QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    isBlogFound = false;
    currentBlogName = "";
    isCurrentBlogWPHosted = false;
    currentBlogPagesCount = 0;
    currentBlogPostsCount = 0;
    currentBlogCommentsCount = 0;
    currentBlogMediaCount = 0;
    imgReader = new QImageReader();
    imgWriter = new QImageWriter();

    iNetworkEngine = NULL;
    /* Data model, needs to be constructed before list views. */
    iNetworkEngine = new WPNetworkEngine();
    iBlogs = new WPDataUsersBlogs(iNetworkEngine);
    iGravatars = new WPDataGravatars(iNetworkEngine);

    QHash<int, QByteArray> blogsRoleNames;
    blogsRoleNames[BlogEntry::TitleRole] = "title";
    blogsRoleNames[BlogEntry::IsAdminRole] = "isAdmin";
    blogsRoleNames[BlogEntry::IdRole] = "blogId";
    blogsRoleNames[BlogEntry::UrlRole] = "blogUrl";
    blogsRoleNames[BlogEntry::UsernameRole] = "username";
    blogsRoleNames[BlogEntry::PasswordRole] = "password";
    blogsRoleNames[BlogEntry::IsWordpressRole] = "isWordpress";
    blogsRoleNames[BlogEntry::PagesCountRole] = "pagesCount";
    blogsRoleNames[BlogEntry::PostsCountRole] = "postsCount";
    blogsRoleNames[BlogEntry::CommentsCountRole] = "commentsCount";

    blogsModel = new RoleItemModel(blogsRoleNames);

    QHash<int, QByteArray> pagesRoleNames;
    pagesRoleNames[PagesEntry::TitleRole] = "title";
    pagesRoleNames[PagesEntry::LocalIdRole] = "localId";
    pagesRoleNames[PagesEntry::DescRole] = "description";
    pagesRoleNames[PagesEntry::LinkRole] = "link";
    pagesRoleNames[PagesEntry::PermaLinkRole] = "permalink";
    pagesRoleNames[PagesEntry::WpAuthorRole] = "wpAuthor";
    pagesRoleNames[PagesEntry::WpAuthorDisplayNameRole] = "wpAuthorDisplayName";
    pagesRoleNames[PagesEntry::WpPageParentTitleRole] = "parentTitle";
    pagesRoleNames[PagesEntry::StatusRole] = "publishStatus";
    pagesRoleNames[PagesEntry::PasswordRole] = "pagePassword";
    pagesRoleNames[PagesEntry::DateCreatedRole] = "dateCreated";
    pagesRoleNames[PagesEntry::PageTimeRole] = "pageTime";
    pagesRoleNames[PagesEntry::PageDateRole] = "pageDate";

    pagesModel = new RoleItemModel(pagesRoleNames);

    QHash<int, QByteArray> postsRoleNames;
    postsRoleNames[PostsEntry::TitleRole] = "title";
    postsRoleNames[PostsEntry::LocalIdRole] = "localId";
    postsRoleNames[PostsEntry::DescRole] = "description";
    postsRoleNames[PostsEntry::LinkRole] = "link";
    postsRoleNames[PostsEntry::PermaLinkRole] = "permalink";
    postsRoleNames[PostsEntry::WpAuthorDisplayNameRole] = "wpAuthorDisplayName";
    postsRoleNames[PostsEntry::CategoriesRole] = "categories";
    postsRoleNames[PostsEntry::CommentsCountRole] = "commentsCount";
    postsRoleNames[PostsEntry::StatusRole] = "publishStatus";
    postsRoleNames[PostsEntry::PasswordRole] = "postPassword";
    postsRoleNames[PostsEntry::DateCreatedRole] = "dateCreated";
    postsRoleNames[PostsEntry::PostTimeRole] = "postTime";
    postsRoleNames[PostsEntry::PostDateRole] = "postDate";

    postsModel = new RoleItemModel(postsRoleNames);

    QHash<int, QByteArray> commentsRoleNames;
    commentsRoleNames[CommentsEntry::IdRole] = "localId";
    commentsRoleNames[CommentsEntry::UserIdRole] = "userId";
    commentsRoleNames[CommentsEntry::ContentRole] = "content";
    commentsRoleNames[CommentsEntry::ContentPreviewRole] = "preview";
    commentsRoleNames[CommentsEntry::PostIdRole] = "postId";
    commentsRoleNames[CommentsEntry::PostTitleRole] = "postTitle";
    commentsRoleNames[CommentsEntry::AuthorRole] = "authorName";
    commentsRoleNames[CommentsEntry::AuthorUrlRole] = "authorUrl";
    commentsRoleNames[CommentsEntry::AuthorEmailRole] = "authorEmail";
    commentsRoleNames[CommentsEntry::AuthorIpRole] = "authorIp";
    commentsRoleNames[CommentsEntry::StatusRole] = "commentStatus";
    commentsRoleNames[CommentsEntry::GravatarUrlRole] = "gravatarUrl";
    commentsRoleNames[CommentsEntry::DateCreatedRole] = "dateCreated";

    commentsModel = new RoleItemModel(commentsRoleNames);
    postCommentsModel = new RoleItemModel(commentsRoleNames);

    QHash<int, QByteArray> categoryRoleNames;
    categoryRoleNames[CategoryEntry::CategoryIdRole] = "catId";
    categoryRoleNames[CategoryEntry::DescriptionRole] = "catDesc";
    categoryRoleNames[CategoryEntry::CategoryNameRole] = "catName";
    categoryRoleNames[CategoryEntry::MarkedRole] = "isMarked";

    categoriesModel = new RoleItemModel(categoryRoleNames);

    QHash<int, QByteArray> mediaRoleNames;
    mediaRoleNames[MediaEntry::TitleRole] = "mediaTitle";
    mediaRoleNames[MediaEntry::DescriptionRole] = "description";
    mediaRoleNames[MediaEntry::CaptionRole] = "caption";
    mediaRoleNames[MediaEntry::LinkRole] = "link";
    mediaRoleNames[MediaEntry::IsThumbnailRole] = "isThumbnail";
    mediaRoleNames[MediaEntry::MetaDataRole] = "metadata";
    mediaRoleNames[MediaEntry::ParentRole] = "parentId";
    mediaRoleNames[MediaEntry::FileTypeRole] = "fileType";
    mediaRoleNames[MediaEntry::DateCreatedRole] = "dateCreated";

    mediaModel = new RoleItemModel(mediaRoleNames);

    QHash<int, QByteArray> localMediaRoleNames;
    localMediaRoleNames[LocalMediaEntry::FileNameRole] = "fileName";
    localMediaRoleNames[LocalMediaEntry::FilePathRole] = "filePath";
    localMediaRoleNames[LocalMediaEntry::FileSizeRole] = "fileSize";
    localMediaRoleNames[LocalMediaEntry::FileTypeRole] = "fileType";
    localMediaRoleNames[LocalMediaEntry::FileThumbRole] = "fileThumbnail";

    localMediaModel = new RoleItemModel(localMediaRoleNames);

    connect(
        iBlogs,         SIGNAL(UpdateBlogs(const WPDataUsersBlogs &)),
        this, SLOT(updateBlogs(const WPDataUsersBlogs &))
    );


    // data changed, update views
    connect(
        iBlogs, SIGNAL(UpdatePageDataFull(const WPDataBlog &)),
        this, SLOT(updatePageData(const WPDataBlog &))
    );

    connect(
        iBlogs, SIGNAL(UpdatePostDataFull(const WPDataBlog &)),
        this, SLOT(updatePostsData(const WPDataBlog &))
    );

    connect(
        iBlogs, SIGNAL(UpdateCommentDataFull(const WPDataBlog &)),
        this, SLOT(updateCommentsData(const WPDataBlog &))
    );

    connect(
        iBlogs, SIGNAL(UpdateCategoryData(const WPDataBlog &)),
        this, SLOT(updateCategoryData(const WPDataBlog &))
    );

    connect(
        iBlogs, SIGNAL(UpdateMediaData(const WPDataBlog &)),
        this, SLOT(updateMediaData(const WPDataBlog &))
    );

    connect(
        iNetworkEngine, SIGNAL(TaskFailed (TWPNetworkEngineCommand, QString)),
        this, SLOT(taskFailed (TWPNetworkEngineCommand, QString))
    );

    // Progress dialog
    connect(
        iNetworkEngine, SIGNAL(TaskStarted(TWPNetworkEngineCommand)),
        this, SLOT(taskStarted(TWPNetworkEngineCommand))
    );

    connect(
        iNetworkEngine, SIGNAL(TaskFinished(TWPNetworkEngineCommand)),
        this, SLOT(taskFinished(TWPNetworkEngineCommand))
    );

    connect(
                this, SIGNAL(uploadFile(WPMediaFile *)),
        iNetworkEngine, SLOT(UploadFile(WPMediaFile *))
    );

    connect(
        iNetworkEngine, SIGNAL(FileUploaded(WPMediaFile)),
                this, SLOT(fileUploaded(WPMediaFile))
    );

    connect(
        iNetworkEngine, SIGNAL(FileUploadFailed(WPMediaFile)),
        this, SLOT(fileUploadFailed(WPMediaFile))
    );

    /*connect(iNetworkEngine,SIGNAL(PageDeleted(int)),this,SLOT(pageDeleted(int)));
    connect(iNetworkEngine,SIGNAL(PostDeleted(int)),this,SLOT(postDeleted(int)));
    connect(iNetworkEngine,SIGNAL(CommentDeleted(int)),this,SLOT(commentDeleted(int)));
    connect(iNetworkEngine,SIGNAL(CommentEdited(int)),this,SLOT(commentEdited(int)));
    connect(iNetworkEngine,SIGNAL(CommentCreated(int,QString)),this,SLOT(commentCreated(int,QString)));*/


    if (iBlogs->LoadLocalData()) {
        qDebug()<<"Abhishek";
        isBlogFound = true;
        emit blogFound(isBlogFound);
        // show cached data, while we refresh it in the background
        //WPDataBlog *blog = iBlogs->GetCurrentBlog();
        //updateBlogs(*iBlogs);
        //updatePageData(*blog);
        //updatePostsData(*blog);
        //updateCommentsData(*blog);

    }
    else
    {
        emit blogFound(false);
    }
    addPostState = ProgressEntry::None;
    addPostStatus = "";
}

Worker::~Worker()
{
    if (iBlogs)
        iBlogs->SaveLocalData();

    delete iGravatars;
    delete iBlogs;
    delete iNetworkEngine;

//    if (searchThread)
//    {
//        if (searchThread->isRunning())
//        {
//            searchThread->Cancel();
//            // Bump priority to make it terminate fast
//            searchThread->setPriority(QThread::HighestPriority);
//            searchThread->wait();
//        }
//        delete searchThread;
//        searchThread = 0;
//    }
}

void Worker::addNewBlog(QString blogUrl,
                        QString username,
                        QString password,
                        bool resizePhotosCheck)
{
    // build WPNetworkEndpoint for new blog settings
    WPNetworkEndpoint settings;

    settings.iBlogUrl = blogUrl;     // must be the same for edited blog
    settings.iUsername = username;
    settings.iPassword = password;
    settings.iNumberOfPosts = QString().number(50);
    settings.iResizePhotos = resizePhotosCheck;
    iBlogs->NewBlog(settings);

}

void Worker::editBlog(QString id,
                      QString blogUrl,
                      QString username,
                      QString password,
                      bool resizePhotosCheck)
{
    // build WPNetworkEndpoint for new blog settings
    if(iBlogs->GetBlog(id) && iBlogs->GetBlog(id)->iEndpoint.iBlogUrl == blogUrl){
        WPNetworkEndpoint settings;

        settings.iBlogUrl = blogUrl;     // must be the same for edited blog
        settings.iUsername = username;
        settings.iPassword = password;
        settings.iNumberOfPosts = QString().number(50);
        settings.iResizePhotos = resizePhotosCheck;
        iBlogs->EditBlog(iBlogs->GetBlog(id), settings);
    }

}

void Worker::updateBlogs(const WPDataUsersBlogs& aUsersBlogs)
{
    qDebug()<<"Worker--------->Updating Blogs";
    currentBlogName = iBlogs->GetCurrentBlog()->iBlogName;
    isCurrentBlogWPHosted = iBlogs->GetCurrentBlog()->isWordpressHosted();
    emit updateCurrentBlogDetails(currentBlogName, isCurrentBlogWPHosted);
    if(aUsersBlogs.GetBlogCount()>blogsModel->rowCount()){
        qDebug()<<"New Bloog Added";
        emit getUserBlogSuccessful();
    }
    blogsModel->clear();
    tempBlogIdHash.clear();

    for (QListIterator<WPDataBlog*> i(aUsersBlogs.GetBlogs()); i.hasNext(); ) {
        WPDataBlog *blog = i.next();
        qDebug()<<blog->iBlogName<<blog->iIsAdmin<<"BloogId"<<blog->LocalId()<<blog->iIsAdmin;
        QStandardItem* it = new QStandardItem();
        it->setData(blog->iBlogName, BlogEntry::TitleRole);
        it->setData(blog->iIsAdmin=="1", BlogEntry::IsAdminRole);
        it->setData(blog->iEndpoint.iBlogUrl, BlogEntry::UrlRole);
        it->setData(blog->iEndpoint.iUsername, BlogEntry::UsernameRole);
        it->setData(blog->iEndpoint.iPassword, BlogEntry::PasswordRole);
        it->setData(blog->isWordpressHosted(), BlogEntry::IsWordpressRole);
        it->setData(blog->LocalId(), BlogEntry::IdRole);
        it->setData("", BlogEntry::PagesCountRole);
        it->setData("", BlogEntry::PostsCountRole);
        it->setData("", BlogEntry::CommentsCountRole);
        tempBlogIdHash.insert(blog->LocalId(), blogsModel->rowCount());
        blogsModel->appendRow(it);
    }
    //blogsModel->setSortRole(BlogEntry::TitleRole);
    //blogsModel->sort(0, Qt::AscendingOrder);
}


void Worker::updatePageData(const WPDataBlog& aBlog)
{
    qDebug()<<"Worker--------->Updating Pages";
    if(iCurrentBlog) {
        disconnect(iCurrentBlog, SIGNAL(PageRemoved(WPPage)), this, NULL);
    }
    pageModelStatus = "Updating pages...";
    pageModelState = ProgressEntry::Processing;
    emit setPageModelStatus(pageModelState, pageModelStatus);
    pagesModel->clear();
    tempPagesIdHash.clear();

    QListIterator<WPPage> pages(aBlog.GetPages());
    currentBlogPagesCount = 0;
    while (pages.hasNext()) {
        WPPage page = pages.next();
        qDebug()<<"MyDebug-Pages>>>>>>>>>>>>>>>>>>>>>"<<page->iTitle<<page->iWpAuthor<<page->iDateCreated<<page->iWpPassword;
        QStandardItem* it = new QStandardItem();
        it->setData(page->iTitle, PagesEntry::TitleRole);
        it->setData(page->LocalId(), PagesEntry::LocalIdRole);
        it->setData(page->iDescription, PagesEntry::DescRole);
        it->setData(page->iLink, PagesEntry::LinkRole);
        it->setData(page->iPermaLink, PagesEntry::PermaLinkRole);
        it->setData(page->iWpAuthor, PagesEntry::WpAuthorRole);
        it->setData(page->iWpAuthorDisplayName, PagesEntry::WpAuthorDisplayNameRole);
        it->setData(page->iWpPageParentTitle, PagesEntry::WpPageParentTitleRole);
        QString status;
        if(page->iStatus=="publish"){
            status = "Published";
//            if(page->iDateCreated > QDateTime::currentDateTime())
//                status = "Scheduled in future";
//            if(page->iWpPassword!="")
//                status = "Protected";
        }
        else if(page->iStatus=="private")
            status = "Private";
        else if(page->iStatus=="pending")
            status = "Pending review";
        else if(page->iStatus=="future")
            status = "Scheduled in future";
        else if(page->iStatus=="draft")
            status = "Draft";
        else if(page->iStatus=="trash")
            status = "Trashed";
        if(page->IsLocal())
            status = "Local Draft";
        it->setData(status, PagesEntry::StatusRole);
        qDebug()<<"Paages iStatus"<<page->iStatus<<it->data(PagesEntry::StatusRole);
        it->setData(page->iWpPassword, PagesEntry::PasswordRole);
        it->setData(page->iDateCreated.toString(), PagesEntry::DateCreatedRole);
        it->setData(page->iDateCreated.time().toString("h:m:s"), PagesEntry::PageTimeRole);
        it->setData(page->iDateCreated.date().toString("yyyy/M/d"), PagesEntry::PageDateRole);
        it->setData(page->iDateCreated.toMSecsSinceEpoch(), PagesEntry::SortDateRole);
        tempPagesIdHash.insert(page->LocalId(), pagesModel->rowCount());
        pagesModel->appendRow(it);
    }
    currentBlogPagesCount = pagesModel->rowCount();
    if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
        blogsModel->item(tempBlogIdHash.value(aBlog.LocalId()))->setData(currentBlogPagesCount, BlogEntry::PagesCountRole);
        emit updateCurrentBlogPagesCount(currentBlogPagesCount);
    }
    iCurrentBlog = const_cast<WPDataBlog *>(&aBlog);

    // signals
    connect(
        &aBlog,  SIGNAL(PageAdded(WPPage)),
        this,    SLOT(pageAdded(WPPage))
    );
    connect(
        &aBlog,  SIGNAL(PageChanged(WPPage)),
        this,    SLOT(pageChanged(WPPage))
    );
    connect(
        &aBlog,  SIGNAL(PageRemoved(WPPage)),
        this,    SLOT(pageRemoved(WPPage))
    );
    pagesModel->setSortRole(PagesEntry::SortDateRole);
    pagesModel->sort(0, Qt::DescendingOrder);
    pageModelStatus = "Pages("+QString().number(currentBlogPagesCount)+")";
    pageModelState = ProgressEntry::Success;
    emit setPageModelStatus(pageModelState, pageModelStatus);
}

void Worker::updatePostsData(const WPDataBlog& aBlog)
{
    qDebug()<<"Worker--------->Updating Posts";
    if(iCurrentBlog) {
        disconnect(iCurrentBlog, SIGNAL(PostRemoved(WPPost)), this, NULL);
    }
    postModelStatus = "Updating posts...";
    postModelState = ProgressEntry::Processing;
    emit setPostModelStatus(postModelState, postModelStatus);
    postsModel->clear();
    tempPostsIdHash.clear();

    QListIterator<WPPost> posts(aBlog.GetPosts());
    currentBlogPostsCount = 0;
    while (posts.hasNext()) {
        WPPost post = posts.next();
        if(post->IsDraft())
            qDebug()<<"Poooooost";
        qDebug()<<"MyDebug-Posts>>>>>>>>>>>>>>>>>>>>>"<<post->iTitle<<post->iWpAuthorDisplayName<<post->iDateCreated<<post->GetComments().count();
        QStandardItem* it = new QStandardItem();
        it->setData(post->iTitle, PostsEntry::TitleRole);
        it->setData(post->LocalId(), PostsEntry::LocalIdRole);
        it->setData(post->iDescription, PostsEntry::DescRole);
        it->setData(post->iLink, PostsEntry::LinkRole);
        it->setData(post->iPermaLink, PostsEntry::PermaLinkRole);
        it->setData(post->iWpAuthorDisplayName, PostsEntry::WpAuthorDisplayNameRole);
        it->setData(post->iCategories.join(", "), PostsEntry::CategoriesRole);
        it->setData(post->GetComments().count(), PostsEntry::CommentsCountRole);
        qDebug()<<"Poost iStatus"<<post->iStatus;
        QString status;
        if(post->iStatus=="publish"){
            status = "Published";
//            if(post->iDateCreated > QDateTime::currentDateTime())
//                status = "Scheduled in future";
//            if(post->iWpPassword!="")
//                status = "Protected";
        }
        else if(post->iStatus=="private")
            status = "Private";
        else if(post->iStatus=="pending")
            status = "Pending review";
        else if(post->iStatus=="future")
            status = "Scheduled in future";
        else if(post->iStatus=="draft")
            status = "Draft";
        else if(post->iStatus=="trash")
            status = "Trashed";
        if(post->IsLocal())
            status = "Local Draft";
//        if(post->IsOrphaned())
//            status = "Orphaned";
//        if(post->IsBusy())
//            status = "Busy";
        it->setData(status, PostsEntry::StatusRole);
        it->setData(post->iWpPassword, PostsEntry::PasswordRole);
        it->setData(post->iDateCreated.toString(Qt::DefaultLocaleShortDate), PostsEntry::DateCreatedRole);
        it->setData(post->iDateCreated.time().toString("h:m:s"), PostsEntry::PostTimeRole);
        it->setData(post->iDateCreated.date().toString("yyyy/M/d"), PostsEntry::PostDateRole);
        it->setData(post->iDateCreated.toMSecsSinceEpoch(), PostsEntry::SortDateRole);
        tempPostsIdHash.insert(post->LocalId(), postsModel->rowCount());
        postsModel->appendRow(it);
        qDebug()<<"tempPostsIdHash"<<post->LocalId()<<postsModel->rowCount();
    }
    currentBlogPostsCount = postsModel->rowCount();
    if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
        blogsModel->item(tempBlogIdHash.value(aBlog.LocalId()))->setData(currentBlogPostsCount, BlogEntry::PostsCountRole);
        emit updateCurrentBlogPostsCount(currentBlogPostsCount);
    }
    iCurrentBlog = const_cast<WPDataBlog *>(&aBlog);

    //signals

    connect(
       &aBlog,  SIGNAL(PostAdded(WPPost)),
       this,    SLOT(postAdded(WPPost))
    );
    connect(
       &aBlog,  SIGNAL(PostChanged(WPPost)),
       this,    SLOT(postChanged(WPPost))
    );
    connect(
       &aBlog,  SIGNAL(PostRemoved(WPPost)),
       this,    SLOT(postRemoved(WPPost))
    );

    postsModel->setSortRole(PostsEntry::SortDateRole);
    postsModel->sort(0, Qt::DescendingOrder);
    postModelStatus = "Posts("+QString().number(currentBlogPostsCount)+")";
    postModelState = ProgressEntry::Success;
    emit setPostModelStatus(postModelState, postModelStatus);
}

void Worker::updateCommentsData(const WPDataBlog& aBlog)
{
    qDebug()<<"Worker--------->Updating Comments";
    if(iCurrentBlog) {
        disconnect(iCurrentBlog, SIGNAL(CommentAdded(WPComment)), this, NULL);
        disconnect(iCurrentBlog, SIGNAL(CommentChanged(WPComment)), this, NULL);
        disconnect(iCurrentBlog, SIGNAL(CommentRemoved(WPComment)), this, NULL);
    }
    commentModelStatus = "Updating comments...";
    commentModelState = ProgressEntry::Processing;
    emit setCommentModelStatus(commentModelState, commentModelStatus);
    commentsModel->clear();
    tempCommentsIdHash.clear();

    // add comments, inserting labels as needed
    QListIterator<WPComment> data(aBlog.GetComments());
    currentBlogCommentsCount = 0;
    while (data.hasNext()) {
        WPComment comment = data.next();
        qDebug()<<"MyDebug-Comments>>>>>>>>>>>>>>>>>>>>>"<<comment->iAuthor<<comment->iAuthorEmail<<comment->iAuthorUrl<<comment->iStatus;
        QStandardItem* it = new QStandardItem();
        it->setData(comment->LocalId(), CommentsEntry::IdRole);
        it->setData(comment->iUserId, CommentsEntry::UserIdRole);
        it->setData(comment->iContent, CommentsEntry::ContentRole);
        QString preview = comment->iContent;
        if(preview.length()>64)
            preview = preview.left(64)+"...";
        it->setData(preview, CommentsEntry::ContentPreviewRole);
        it->setData(comment->iPostId, CommentsEntry::PostIdRole);
        it->setData(comment->iPostTitle, CommentsEntry::PostTitleRole);
        it->setData(comment->iAuthor, CommentsEntry::AuthorRole);
        it->setData(comment->iAuthorEmail, CommentsEntry::AuthorEmailRole);
        it->setData(comment->iAuthorUrl, CommentsEntry::AuthorUrlRole);
        it->setData(comment->iAuthorIp, CommentsEntry::AuthorIpRole);
        it->setData(comment->iStatus, CommentsEntry::StatusRole);
        QByteArray gravatarEmail = comment->iAuthorEmail.trimmed().toUtf8();
        qDebug()<<"Hassh"<<comment->iAuthorEmail.trimmed().toUtf8()<<QCryptographicHash::hash (gravatarEmail, QCryptographicHash::Md5 ).toHex();
        it->setData("http://www.gravatar.com/avatar/"+QString(QCryptographicHash::hash (comment->iAuthorEmail.trimmed().toUtf8(), QCryptographicHash::Md5 ).toHex())+"?s=64&d=mm", CommentsEntry::GravatarUrlRole);
        it->setData(comment->iDateCreated.toString(Qt::DefaultLocaleShortDate), CommentsEntry::DateCreatedRole);
        it->setData(comment->iDateCreated.toMSecsSinceEpoch(), CommentsEntry::SortDateRole);
        tempCommentsIdHash.insert(comment->LocalId(), commentsModel->rowCount());
        commentsModel->appendRow(it);
    }
    currentBlogCommentsCount = commentsModel->rowCount();
    if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
        qDebug()<<"Count"<<currentBlogCommentsCount<<blogsModel->item(tempBlogIdHash.value(aBlog.LocalId()))->data(BlogEntry::TitleRole);
        blogsModel->item(tempBlogIdHash.value(aBlog.LocalId()))->setData(currentBlogCommentsCount, BlogEntry::CommentsCountRole);
        emit updateCurrentBlogCommentsCount(currentBlogCommentsCount);
    }
    iCurrentBlog = const_cast<WPDataBlog *>(&aBlog);

    //signals
    connect(
        &aBlog,  SIGNAL(CommentAdded(WPComment)),
        this,   SLOT(commentAdded(WPComment))
    );
    connect(
        &aBlog,  SIGNAL(CommentChanged(WPComment)),
        this,   SLOT(commentChanged(WPComment))
    );
    connect(
        &aBlog,  SIGNAL(CommentRemoved(WPComment)),
        this,   SLOT(commentRemoved(WPComment))
    );
    commentsModel->setSortRole(CommentsEntry::SortDateRole);
    commentsModel->sort(0, Qt::DescendingOrder);
    commentModelStatus = "Comments("+QString().number(currentBlogCommentsCount)+")";
    commentModelState = ProgressEntry::Success;
    emit setCommentModelStatus(commentModelState, commentModelStatus);
}

void Worker::updateCategoryData(const WPDataBlog & aBlog)
{
    qDebug()<<"Worker--------->Updating Categories";
    if(iCurrentBlog) {
        disconnect(iCurrentBlog, SIGNAL(CategoryAdded(WPDataCategory *)), this, NULL);
    }

    categoriesModel->clear();

    for (QListIterator<WPDataCategory> i(aBlog.iCategories); i.hasNext(); ) {
        const WPDataCategory &category = i.next();

        qDebug()<<"MyDebug-Categories>>>>>>>>>>>>>>>>>>>>>"<<category.iCategoryName<<category.iCategoryDescription<<category.iDescription;
        QStandardItem* it = new QStandardItem();
        it->setData(category.iCategoryId, CategoryEntry::CategoryIdRole);
        it->setData(category.iCategoryName, CategoryEntry::CategoryNameRole);
        it->setData(category.iCategoryDescription, CategoryEntry::DescriptionRole);
        it->setData(false, CategoryEntry::MarkedRole);
        categoriesModel->appendRow(it);
    }

    iCurrentBlog = const_cast<WPDataBlog *>(&aBlog);

    // XXX: maintain signals
    connect(
        iCurrentBlog,   SIGNAL(CategoryAdded(WPDataCategory *)),
        this,             SLOT(categoryAdded(WPDataCategory *))
    );
    categoriesModel->setSortRole(CategoryEntry::CategoryNameRole);
    categoriesModel->sort(0, Qt::AscendingOrder);
}

void Worker::updateMediaData(const WPDataBlog & aBlog)
{
    qDebug()<<"Worker--------->Updating Media Items";
    if(iCurrentBlog) {
        //disconnect(iCurrentBlog, SIGNAL(CategoryAdded(WPDataCategory *)), this, NULL);
    }

    mediaModel->clear();

    for (QListIterator<WPDataMediaItem> i(aBlog.iMediaLibrary); i.hasNext(); ) {
        const WPDataMediaItem &mediaItem = i.next();

        qDebug()<<"MyDebug-Categories>>>>>>>>>>>>>>>>>>>>>"<<mediaItem.iTitle<<mediaItem.iDateCreated<<mediaItem.iLink;
        QStandardItem* it = new QStandardItem();
        it->setData(mediaItem.iTitle, MediaEntry::TitleRole);
        it->setData(mediaItem.iDescription, MediaEntry::DescriptionRole);
        it->setData(mediaItem.iCaption, MediaEntry::CaptionRole);
        it->setData(mediaItem.iThumbnail, MediaEntry::IsThumbnailRole);
        it->setData(mediaItem.iParentId, MediaEntry::ParentRole);
        it->setData(mediaItem.iMetaData, MediaEntry::MetaDataRole);
        it->setData(mediaItem.iLink, MediaEntry::LinkRole);
        QString type = mediaItem.iLink;
        type = type.split("/").last().split(".").last().toLower();
        if(type=="jpg"||type=="jpeg"||type=="png"||type=="bmp")
            type = "image";
        else if(type=="mp4"||type=="flv"||type=="avi"||type=="mpeg"||type=="mpg")
            type = "video";
        else
            type = "file";
        it->setData(type, MediaEntry::FileTypeRole);
        it->setData(mediaItem.iDateCreated.toString(Qt::SystemLocaleShortDate), MediaEntry::DateCreatedRole);
        it->setData(mediaItem.iDateCreated.toMSecsSinceEpoch(), MediaEntry::SortDateRole);
        mediaModel->appendRow(it);
    }
    mediaModel->setSortRole(MediaEntry::SortDateRole);
    mediaModel->sort(0, Qt::DescendingOrder);

    currentBlogMediaCount = mediaModel->rowCount();
    emit updateCurrentBlogMediaCount(currentBlogMediaCount);
    iCurrentBlog = const_cast<WPDataBlog *>(&aBlog);
    mediaModelStatus = "Media Items";
    mediaModelState = ProgressEntry::Success;
    emit setMediaModelStatus(mediaModelState, mediaModelStatus);
}

void Worker::checkBlog()
{
    qDebug()<<"Hello Abhishek";
    if(isBlogFound)
        emit blogFound(true);
    else
        emit blogFound(false);
}

void Worker::openBlog(QString id)
{
    qDebug()<<"Changing blog"<<id;//<<iBlogs->GetBlog(id)->iBlogName;
    iBlogs->OpenBlog(iBlogs->GetBlog(id));
    currentBlogName = iBlogs->GetCurrentBlog()->iBlogName;
    isCurrentBlogWPHosted = iBlogs->GetCurrentBlog()->isWordpressHosted();
    emit updateCurrentBlogDetails(currentBlogName, isCurrentBlogWPHosted);
}

void Worker::refreshCurrentBlog()
{
    qDebug()<<"Refresh Bloog";
    iBlogs->GetCurrentBlog()->RefreshAll();
}

void Worker::refreshCurrentBlogPages()
{
    qDebug()<<"Refresh Paages";
    iBlogs->GetCurrentBlog()->iPages.RefreshAll();
}

void Worker::refreshCurrentBlogPosts()
{
    qDebug()<<"Refresh Poosts";
    iBlogs->GetCurrentBlog()->iPosts.RefreshAll();
}

void Worker::refreshCurrentBlogComments()
{
    qDebug()<<"Refresh Coomments";
    iBlogs->GetCurrentBlog()->iComments.RefreshAll();
}

void Worker::refreshCurrentBlogMedia()
{
    qDebug()<<"Refresh Media";
    iBlogs->GetCurrentBlog()->RefreshMediaLibrary();
}

void Worker::openPost(QString id)
{
    qDebug()<<"Open Poost"<<id;
    WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(id);
    QString status;
    if(post->iStatus=="publish"){
//        if(post->iDateCreated > QDateTime::currentDateTime())
//            status = "Scheduled in future";
//        else
            status = "Published";
    }
    else if(post->iStatus=="private")
        status = "Private";
    else if(post->iStatus=="pending")
        status = "Pending review";
    else if(post->iStatus=="future")
        status = "Scheduled in future";
    else if(post->iStatus=="draft")
        status = "Draft";
    else if(post->iStatus=="trash")
        status = "Trashed";
    if(post->IsLocal())
        status = "Local Draft";
    if(post->IsOrphaned())
        status = "Orphaned";
    if(post->IsBusy())
        status = "Busy";
    postCommentsModel->clear();
    emit showPost(id,
                  post->iTitle,
                  post->iDescription,
                  post->iDateCreated.toString(Qt::SystemLocaleShortDate),
                  post->iCategories.join(", "),
                  post->iLink,
                  post->GetComments().count(),
                  post->iWpAuthorDisplayName,
                  post->iWpPassword,
                  status);
    preparePostCommentsModel(id);
    qDebug()<<"Open Poost to UI"<<id;
}

void Worker::replyToComment(QString id, QString pId)
{
    qDebug()<<"Reply to cooment"<<id;
    this->openPost(iBlogs->GetCurrentBlog()->iPosts.GetByItemId(pId)->LocalId());
}

void Worker::taskStarted(TWPNetworkEngineCommand command)
{
    if(command == EGetPages || command == EGetPageStatuses || command == EGetPage){
        qDebug()<<"Network--------->Updating Pages";
        pageModelStatus = "Synchronizing pages...";
        pageModelState = ProgressEntry::Processing;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
    else if(command == EDeletePage){
        pageModelStatus = "Deleting selected pages...";
        pageModelState = ProgressEntry::Processing;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
    else if(command == EGetPosts || command == EGetPostStatuses || command == EGetPost){
        qDebug()<<"Network--------->Updating Posts";
        postModelStatus = "Synchronizing posts...";
        postModelState = ProgressEntry::Processing;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
    else if(command == EDeletePost){
        postModelStatus = "Deleting selected posts...";
        postModelState = ProgressEntry::Processing;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
    else if(command == EGetComments || command == EGetCommentStatuses || command == EGetComment){
        qDebug()<<"Network--------->Updating Comments";
        commentModelStatus = "Synchronizing comments...";
        commentModelState = ProgressEntry::Processing;
        emit setCommentModelStatus(commentModelState, commentModelStatus);
    }
    else if(command == EDeleteComment){
        commentModelStatus = "Deleting selected comments...";
        commentModelState = ProgressEntry::Processing;
        emit setCommentModelStatus(commentModelState, commentModelStatus);
    }
    else if(command == EGetMediaLibrary){
        qDebug()<<"Network--------->Updating Media Library";
        mediaModelStatus = "Synchronizing media items...";
        mediaModelState = ProgressEntry::Processing;
        emit setMediaModelStatus(mediaModelState, mediaModelStatus);
    }
    else if(command == EUploadFile){
        qDebug()<<"Network--------->Uploading file";
        addMediaStatus = "Uploading item...";
        addMediaState = ProgressEntry::Processing;
        emit setAddMediaStatus(addMediaState, addMediaStatus);
    }
    else if(command == ENewPost){
        qDebug()<<"Network--------->Publishing post";
        addPostState = ProgressEntry::Processing;
        addPostStatus = "Publishing post...";
        emit setAddPostStatus(addPostState, addPostStatus);
    }
    else if(command == EEditPost){
        qDebug()<<"Network--------->Editing post";
        addPostState = ProgressEntry::Processing;
        addPostStatus = "Editing post...";
        emit setAddPostStatus(addPostState, addPostStatus);
    }
    else if(command == ENewPage){
        qDebug()<<"Network--------->Publishing page";
        addPageState = ProgressEntry::Processing;
        addPageStatus = "Publishing page...";
        emit setAddPageStatus(addPageState, addPageStatus);
    }
    else if(command == EEditPage){
        qDebug()<<"Network--------->Editing page";
        addPageState = ProgressEntry::Processing;
        addPageStatus = "Editing post...";
        emit setAddPageStatus(addPostState, addPageStatus);
    }
    else if(command == ENewComment){
        qDebug()<<"Network--------->Adding comment";
        addCommentState = ProgressEntry::Processing;
        addCommentStatus = "Adding comment...";
        emit setAddCommentStatus(addCommentState, addCommentStatus);
    }
    else if(command == ENewCategory){
        qDebug()<<"Network--------->Adding category";
        addCategoryState = 1;
        addCategoryStatus = "Adding category...";
        emit setAddCategoryStatus(addCategoryState, addCategoryStatus);
    }
}

void Worker::taskFinished(TWPNetworkEngineCommand command)
{
    if((command == EGetPages || command == EGetPageStatuses || command == EGetPage) && pageModelState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Updating Pages";
        pageModelStatus = "Synchronizing posts complete...";
        pageModelState = ProgressEntry::Processing;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
    else if((command == EDeletePage) && pageModelState == ProgressEntry::Processing){
        pageModelStatus = "Deleted selected pages...";
        pageModelState = ProgressEntry::Processing;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
    else if((command == EGetPosts || command == EGetPostStatuses || command == EGetPost) && postModelState == ProgressEntry::Processing){
        postModelStatus = "Synchronizing posts complete...";
        postModelState = ProgressEntry::Success;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
    else if((command == EDeletePost) && postModelState == ProgressEntry::Processing){
        postModelStatus = "Deleted selected posts...";
        postModelState = ProgressEntry::Success;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
    else if((command == EGetComments || command == EGetCommentStatuses || command == EGetComment) && commentModelState == ProgressEntry::Processing){
        commentModelStatus = "Synchronizing comments complete...";
        commentModelState = ProgressEntry::Success;
        emit setCommentModelStatus(commentModelState, commentModelStatus);
    }
    else if((command == EDeleteComment) && commentModelState == ProgressEntry::Processing){
        commentModelStatus = "Deleted selected comments...";
        commentModelState = ProgressEntry::Success;
        emit setCommentModelStatus(commentModelState, commentModelStatus);
    }
    else if(command == EGetMediaLibrary && mediaModelState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Updating Media Library Complete";
        mediaModelStatus = "Synchronizing media items complete...";
        mediaModelState = ProgressEntry::Success;
        emit setMediaModelStatus(mediaModelState, mediaModelStatus);
    }
    else if(command == EUploadFile && addMediaState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Upload finished";
        addMediaStatus = "Uploading finished!";
        addMediaState = ProgressEntry::Success;
        emit setAddMediaStatus(addMediaState, addMediaStatus);
    }
    else if(command == ENewPost){
        qDebug()<<"Network--------->New Post Published";
        addPostState = ProgressEntry::Success;
        addPostStatus = "Post published!";
        emit setAddPostStatus(addPostState, addPostStatus);
        emit newPostPublished();
    }
    else if(command == EEditPost){
        qDebug()<<"Network--------->Post Edited";
        addPostState = ProgressEntry::Success;
        addPostStatus = "Post edited!";
        emit setAddPostStatus(addPostState, addPostStatus);
        emit newPostPublished();
    }
    else if(command == ENewPage){
        qDebug()<<"Network--------->New Page Published";
        addPageState = ProgressEntry::Success;
        addPageStatus = "Page published!";
        emit setAddPageStatus(addPageState, addPageStatus);
        emit newPagePublished();
    }
    else if(command == EEditPage){
        qDebug()<<"Network--------->Page Edited";
        addPageState = ProgressEntry::Success;
        addPageStatus = "Page edited!";
        emit setAddPageStatus(addPageState, addPageStatus);
        emit newPagePublished();
    }
    else if(command == ENewComment && addCommentState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Comment added";
        addCommentState = ProgressEntry::Success;
        addCommentStatus = "Comment added";
        emit setAddCommentStatus(addCommentState, addCommentStatus);
    }
    else if(command == ENewCategory && addCategoryState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Category Added";
        addCategoryState = 0;
        addCategoryStatus = "";
        emit setAddCategoryStatus(addCategoryState, addCategoryStatus);
    }
}

void Worker::taskFailed(TWPNetworkEngineCommand command, QString msg)
{
    qDebug()<<"Erroor"<<command<<msg;
    if(command == EGetUsersBlogs || command == EGetMainPage){
        qDebug()<<"Erroor"<<msg;
        emit getUserBlogUnsuccessful(msg);
    }
    else if((command == EGetPages || command == EGetPageStatuses || command == EGetPage) && pageModelState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Updating Pages Failed"<<msg;
        pageModelStatus = "Pages<br>" + msg;
        //pageModelStatus = "Error connecting...";
        pageModelState = ProgressEntry::Error;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
    else if((command == EGetPosts || command == EGetPostStatuses || command == EGetPost) && postModelState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Updating Posts Failed"<<msg;
        postModelStatus = "Posts<br>" + msg;
        postModelState = ProgressEntry::Error;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
    else if((command == EGetComments || command == EGetCommentStatuses || command == EGetComment) && commentModelState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Updating Comments Failed"<<msg;
        commentModelStatus = "Comments<br>" + msg;
        commentModelState = ProgressEntry::Error;
        emit setCommentModelStatus(commentModelState, commentModelStatus);
    }
    else if(command == EGetMediaLibrary && mediaModelState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Updating Media Library Failed"<<msg;
        mediaModelStatus = "Media Items<br>" + msg;
        mediaModelState = ProgressEntry::Error;
        emit setMediaModelStatus(mediaModelState, mediaModelStatus);
    }
    else if(command == EUploadFile && addMediaState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Upload failed"<<msg;
        addMediaStatus = "Uploading failed!<br>" + msg;
        addMediaState = ProgressEntry::Error;
        emit setAddMediaStatus(addMediaState, addMediaStatus);
    }
    else if(command == ENewPost){
        qDebug()<<"Network--------->New Post Publishing Failed"<<msg;
        addPostState = ProgressEntry::Error;
        addPostStatus = "Post publishing failed! Saved as local draft<br>" + msg;
        emit setAddPostStatus(addPostState, addPostStatus);
        emit newPostPublishFailed(postPub);
    }
    else if(command == EEditPost){
        qDebug()<<"Network--------->Post Editing Failed"<<msg;
        addPostState = ProgressEntry::Error;
        addPostStatus = "Post editing failed!<br>" + msg;
        emit setAddPostStatus(addPostState, addPostStatus);
    }
    else if(command == ENewPage){
        qDebug()<<"Network--------->New Page Publishing Failed"<<msg;
        addPageState = ProgressEntry::Error;
        addPageStatus = "Page publishing failed! Saved as local draft<br>" + msg;
        emit setAddPageStatus(addPageState, addPageStatus);
        emit newPagePublishFailed(pagePub);
    }
    else if(command == EEditPage){
        qDebug()<<"Network--------->Page Editing Failed"<<msg;
        addPageState = ProgressEntry::Error;
        addPageStatus = "Page editing failed!<br>" + msg;
        emit setAddPageStatus(addPageState, addPageStatus);
    }
    else if(command == ENewComment && addCommentState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Comment addition failed";
        addCommentState = ProgressEntry::Error;
        addCommentStatus = "Comment addition failed!<br>" + msg;
        emit setAddCommentStatus(addCommentState, addCommentStatus);
    }
    else if(command == ENewCategory && addCategoryState == ProgressEntry::Processing){
        qDebug()<<"Network--------->Adding category failed!"<<msg;
        addCategoryState = 2;
        addCategoryStatus = "Unable to add new category!<br>" + msg;
        emit setAddCategoryStatus(addCategoryState, addCategoryStatus);
    }
}

void Worker::deleteBlog(QString id)
{
    qDebug()<<"DeleteBloog"<<id;
    iBlogs->DeleteBlog(iBlogs->GetBlog(id));
    //blogDeleted(id.toInt());
}

void Worker::deletePage(QString id)
{
    qDebug()<<"DeletePaages"<<id;
    iBlogs->GetCurrentBlog()->iPages.GetByLocalId(id)->Delete();
}

void Worker::deletePost(QString id)
{
    qDebug()<<"DeletePoost"<<id;
    iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(id)->Delete();
}

void Worker::approveComment(QString id)
{
    qDebug()<<"ApproveCoomment"<<id;
    iBlogs->GetCurrentBlog()->iComments.GetByLocalId(id)->UpdateStatus("approve");
}

void Worker::unApproveComment(QString id)
{
    qDebug()<<"UnapproveCoomment"<<id;
    iBlogs->GetCurrentBlog()->iComments.GetByLocalId(id)->UpdateStatus("hold");
}

void Worker::deleteComment(QString id)
{
    qDebug()<<"DeleteCoomment"<<id;
    iBlogs->GetCurrentBlog()->iComments.GetByLocalId(id)->Delete();
}

void Worker::spamComment(QString id)
{
    qDebug()<<"SpamCoomment"<<id;
    iBlogs->GetCurrentBlog()->iComments.GetByLocalId(id)->UpdateStatus("spam");
}

void Worker::addComment(QString postId, QString comment)
{
    qDebug()<<"AddCoomment"<<postId;
    WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(postId);
    post->NewComment(comment);
}

void Worker::blogDeleted(int id)
{
    if(blogsModel->rowCount()>0){
        qDebug()<<"Delete Bloog"<<tempBlogIdHash.value(QString::number(id))<<QString::number(id)<<blogsModel->removeRow(tempBlogIdHash.value(QString::number(id)));
        tempBlogIdHash.remove(QString::number(id));
    }
    else{
        qDebug()<<"Delete Bloog"<<tempBlogIdHash.value(QString::number(id))<<QString::number(id);
        blogsModel->clear();
        tempBlogIdHash.clear();
    }
}

void Worker::pageAdded(WPPage page)
{
    qDebug()<<"Added-Page>>>>>>>>>>>>>>>>>>>>>"<<page->iTitle<<page->iWpAuthor<<page->iDateCreated;
    QStandardItem* it = new QStandardItem();
    it->setData(page->iTitle, PagesEntry::TitleRole);
    it->setData(page->LocalId(), PagesEntry::LocalIdRole);
    it->setData(page->iDescription, PagesEntry::DescRole);
    it->setData(page->iLink, PagesEntry::LinkRole);
    it->setData(page->iPermaLink, PagesEntry::PermaLinkRole);
    it->setData(page->iWpAuthor, PagesEntry::WpAuthorRole);
    it->setData(page->iWpAuthorDisplayName, PagesEntry::WpAuthorDisplayNameRole);
    it->setData(page->iWpPageParentTitle, PagesEntry::WpPageParentTitleRole);
    QString status;
    if(page->iStatus=="publish"){
        status = "Published";
//        if(page->iDateCreated > QDateTime::currentDateTime())
//            status = "Scheduled in future";
//        if(page->iWpPassword!="")
//            status = "Protected";
    }
    else if(page->iStatus=="private")
        status = "Private";
    else if(page->iStatus=="pending")
        status = "Pending review";
    else if(page->iStatus=="future")
        status = "Scheduled in future";
    else if(page->iStatus=="draft")
        status = "Draft";
    else if(page->iStatus=="trash")
        status = "Trashed";
    if(page->IsLocal())
        status = "Local Draft";
    it->setData(status, PagesEntry::StatusRole);
    qDebug()<<"Paages iStatus"<<page->iStatus<<it->data(PagesEntry::StatusRole);
    it->setData(page->iWpPassword, PagesEntry::PasswordRole);
    it->setData(page->iDateCreated.toString(), PagesEntry::DateCreatedRole);
    it->setData(page->iDateCreated.time().toString("h:m:s"), PagesEntry::PageTimeRole);
    it->setData(page->iDateCreated.date().toString("yyyy/M/d"), PagesEntry::PageDateRole);
    it->setData(page->iDateCreated.toMSecsSinceEpoch(), PagesEntry::SortDateRole);
    pagesModel->appendRow(it);
    currentBlogPagesCount = pagesModel->rowCount();
    if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
        blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogPagesCount, BlogEntry::PagesCountRole);
        emit updateCurrentBlogPagesCount(currentBlogPagesCount);
    }
    pagesModel->setSortRole(PagesEntry::SortDateRole);
    pagesModel->sort(0, Qt::DescendingOrder);
    pageModelStatus = "Pages("+QString().number(currentBlogPagesCount)+")";
    pageModelState = ProgressEntry::Success;
    emit setPageModelStatus(pageModelState, pageModelStatus);
}

void Worker::pageChanged(WPPage page)
{
    qDebug()<<"Page Changed"<<page->iTitle;
    if(!page->IsBusy()){
        if(pagesModel->rowCount()>0){
            for(int i=0;i<pagesModel->rowCount();i++)
            {
                if(pagesModel->item(i)->data(PagesEntry::LocalIdRole).toString() == page->LocalId()){
                    qDebug()<<"Changed-Page>>>>>>>>>>>>>>>>>>>>>"<<page->iTitle<<page->iWpAuthor<<page->iDateCreated;
                    pagesModel->removeRow(i);
                    break;
                }
            }
        }
        qDebug()<<"Readded-Page>>>>>>>>>>>>>>>>>>>>>"<<page->iTitle<<page->iWpAuthor<<page->iDateCreated;
        QStandardItem* it = new QStandardItem();
        it->setData(page->iTitle, PagesEntry::TitleRole);
        it->setData(page->LocalId(), PagesEntry::LocalIdRole);
        it->setData(page->iDescription, PagesEntry::DescRole);
        it->setData(page->iLink, PagesEntry::LinkRole);
        it->setData(page->iPermaLink, PagesEntry::PermaLinkRole);
        it->setData(page->iWpAuthor, PagesEntry::WpAuthorRole);
        it->setData(page->iWpAuthorDisplayName, PagesEntry::WpAuthorDisplayNameRole);
        it->setData(page->iWpPageParentTitle, PagesEntry::WpPageParentTitleRole);
        QString status;
        if(page->iStatus=="publish"){
            status = "Published";
//            if(page->iDateCreated > QDateTime::currentDateTime())
//                status = "Scheduled in future";
//            if(page->iWpPassword!="")
//                status = "Protected";
        }
        else if(page->iStatus=="private")
            status = "Private";
        else if(page->iStatus=="pending")
            status = "Pending review";
        else if(page->iStatus=="future")
            status = "Scheduled in future";
        else if(page->iStatus=="draft")
            status = "Draft";
        else if(page->iStatus=="trash")
            status = "Trashed";
        if(page->IsLocal())
            status = "Local Draft";
        it->setData(status, PagesEntry::StatusRole);
        qDebug()<<"Paages iStatus"<<page->iStatus<<it->data(PagesEntry::StatusRole);
        it->setData(page->iWpPassword, PagesEntry::PasswordRole);
        it->setData(page->iDateCreated.toString(), PagesEntry::DateCreatedRole);
        it->setData(page->iDateCreated.time().toString("h:m:s"), PagesEntry::PageTimeRole);
        it->setData(page->iDateCreated.date().toString("yyyy/M/d"), PagesEntry::PageDateRole);
        it->setData(page->iDateCreated.toMSecsSinceEpoch(), PagesEntry::SortDateRole);
        pagesModel->appendRow(it);
        currentBlogPagesCount = pagesModel->rowCount();
        if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
            blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogPagesCount, BlogEntry::PagesCountRole);
            emit updateCurrentBlogPagesCount(currentBlogPagesCount);
        }
        pagesModel->setSortRole(PagesEntry::SortDateRole);
        pagesModel->sort(0, Qt::DescendingOrder);
        pageModelStatus = "Pages("+QString().number(currentBlogPagesCount)+")";
        pageModelState = ProgressEntry::Success;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
}

void Worker::pageRemoved(WPPage page)
{
    if(pagesModel->rowCount()>0){
        for(int i=0;i<pagesModel->rowCount();i++){
            if(pagesModel->item(i)->data(PagesEntry::LocalIdRole).toString()==page->LocalId()){
                qDebug()<<"Delete Paage"<<i<<page->LocalId()<<pagesModel->removeRow(i);
                break;
            }
        }
        tempPagesIdHash.remove(page->LocalId());

        currentBlogPagesCount = pagesModel->rowCount();
        if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
            blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogPagesCount, BlogEntry::PagesCountRole);
            emit updateCurrentBlogPagesCount(currentBlogPagesCount);
        }
        pageModelStatus = "Pages("+QString().number(currentBlogPagesCount)+")";
        pageModelState = ProgressEntry::Success;
        emit setPageModelStatus(pageModelState, pageModelStatus);
    }
    else{
        qDebug()<<"Delete Poost"<<tempPagesIdHash.value(page->LocalId())<<page->LocalId();
        pagesModel->clear();
        tempPagesIdHash.clear();
    }

}

void Worker::postAdded(WPPost post)
{
    qDebug()<<"Added-Post>>>>>>>>>>>>>>>>>>>>>"<<post->iTitle<<post->iWpAuthorDisplayName<<post->iDateCreated<<post->GetComments().count();
    QStandardItem* it = new QStandardItem();
    it->setData(post->iTitle, PostsEntry::TitleRole);
    it->setData(post->LocalId(), PostsEntry::LocalIdRole);
    it->setData(post->iDescription, PostsEntry::DescRole);
    it->setData(post->iLink, PostsEntry::LinkRole);
    it->setData(post->iPermaLink, PostsEntry::PermaLinkRole);
    it->setData(post->iWpAuthorDisplayName, PostsEntry::WpAuthorDisplayNameRole);
    it->setData(post->iCategories.join(", "), PostsEntry::CategoriesRole);
    it->setData(post->GetComments().count(), PostsEntry::CommentsCountRole);
    QString status;
    if(post->iStatus=="publish"){
        status = "Published";
//        if(post->iDateCreated > QDateTime::currentDateTime())
//            status = "Scheduled in future";
//        if(post->iWpPassword!="")
//            status = "Protected";
    }
    else if(post->iStatus=="private")
        status = "Private";
    else if(post->iStatus=="pending")
        status = "Pending review";
    else if(post->iStatus=="future")
        status = "Scheduled in future";
    else if(post->iStatus=="draft")
        status = "Draft";
    else if(post->iStatus=="trash")
        status = "Trashed";
    if(post->IsLocal())
        status = "Local Draft";
//    if(post->IsOrphaned())
//        status = "Orphaned";
//    if(post->IsBusy())
//        status = "Busy";
    it->setData(status, PostsEntry::StatusRole);
    it->setData(post->iWpPassword, PostsEntry::PasswordRole);
    it->setData(post->iDateCreated.toString(Qt::DefaultLocaleShortDate), PostsEntry::DateCreatedRole);
    it->setData(post->iDateCreated.time().toString("h:m:s"), PostsEntry::PostTimeRole);
    it->setData(post->iDateCreated.date().toString("yyyy/M/d"), PostsEntry::PostDateRole);
    it->setData(post->iDateCreated.toMSecsSinceEpoch(), PostsEntry::SortDateRole);
    tempPostsIdHash.insert(post->LocalId(), postsModel->rowCount());
    postsModel->appendRow(it);
    qDebug()<<"tempPostsIdHash"<<post->LocalId()<<postsModel->rowCount();

    currentBlogPostsCount = postsModel->rowCount();
    if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
        blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogPostsCount, BlogEntry::PostsCountRole);
        emit updateCurrentBlogPostsCount(currentBlogPostsCount);
    }
    postsModel->setSortRole(PostsEntry::SortDateRole);
    postsModel->sort(0, Qt::DescendingOrder);
    postModelStatus = "Posts("+QString().number(currentBlogPostsCount)+")";
    postModelState = ProgressEntry::Success;
    emit setPostModelStatus(postModelState, postModelStatus);
}

void Worker::postChanged(WPPost post)
{
    qDebug()<<"Post Changed"<<post->iTitle;
    if(!post->IsBusy()){
        if(postsModel->rowCount()>0){
            for(int i=0;i<postsModel->rowCount();i++)
            {
                if(postsModel->item(i)->data(PostsEntry::LocalIdRole).toString() == post->LocalId()){
                    qDebug()<<"Changed-Post>>>>>>>>>>>>>>>>>>>>>"<<post->iTitle<<post->iWpAuthorDisplayName<<post->iDateCreated<<post->GetComments().count();
                    postsModel->removeRow(i);
                    break;
                }
            }
        }
        qDebug()<<"Readding Post"<<post->iTitle;
        QStandardItem* it = new QStandardItem();
        it->setData(post->iTitle, PostsEntry::TitleRole);
        it->setData(post->LocalId(), PostsEntry::LocalIdRole);
        it->setData(post->iDescription, PostsEntry::DescRole);
        it->setData(post->iLink, PostsEntry::LinkRole);
        it->setData(post->iPermaLink, PostsEntry::PermaLinkRole);
        it->setData(post->iWpAuthorDisplayName, PostsEntry::WpAuthorDisplayNameRole);
        it->setData(post->iCategories.join(", "), PostsEntry::CategoriesRole);
        it->setData(post->GetComments().count(), PostsEntry::CommentsCountRole);
        QString status;
        if(post->iStatus=="publish"){
            status = "Published";
//            if(post->iDateCreated > QDateTime::currentDateTime())
//                status = "Scheduled in future";
//            if(post->iWpPassword!="")
//                status = "Protected";
        }
        else if(post->iStatus=="private")
            status = "Private";
        else if(post->iStatus=="pending")
            status = "Pending review";
        else if(post->iStatus=="future")
            status = "Scheduled in future";
        else if(post->iStatus=="draft")
            status = "Draft";
        else if(post->iStatus=="trash")
            status = "Trashed";
        if(post->IsLocal())
            status = "Local Draft";
    //    if(post->IsOrphaned())
    //        status = "Orphaned";
    //    if(post->IsBusy())
    //        status = "Busy";
        it->setData(status, PostsEntry::StatusRole);
        it->setData(post->iWpPassword, PostsEntry::PasswordRole);
        it->setData(post->iDateCreated.toString(Qt::DefaultLocaleShortDate), PostsEntry::DateCreatedRole);
        it->setData(post->iDateCreated.time().toString("h:m:s"), PostsEntry::PostTimeRole);
        it->setData(post->iDateCreated.date().toString("yyyy/M/d"), PostsEntry::PostDateRole);
        it->setData(post->iDateCreated.toMSecsSinceEpoch(), PostsEntry::SortDateRole);
        tempPostsIdHash.insert(post->LocalId(), postsModel->rowCount());
        postsModel->appendRow(it);
        currentBlogPostsCount = postsModel->rowCount();
        if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
            blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogPostsCount, BlogEntry::PostsCountRole);
            emit updateCurrentBlogPostsCount(currentBlogPostsCount);
        }
        postsModel->setSortRole(PostsEntry::SortDateRole);
        postsModel->sort(0, Qt::DescendingOrder);
        postModelStatus = "Posts("+QString().number(currentBlogPostsCount)+")";
        postModelState = ProgressEntry::Success;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
}

void Worker::postRemoved(WPPost post)
{
    if(postsModel->rowCount()>0){
        for(int i=0;i<postsModel->rowCount();i++){
            if(postsModel->item(i)->data(PostsEntry::LocalIdRole).toString()==post->LocalId()){
                qDebug()<<"Delete Poost"<<i<<post->LocalId()<<postsModel->removeRow(i);
                break;
            }
        }
        tempPostsIdHash.remove(post->LocalId());

        currentBlogPostsCount = postsModel->rowCount();
        if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
            blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogPostsCount, BlogEntry::PostsCountRole);
            emit updateCurrentBlogPostsCount(currentBlogPostsCount);
        }
        postModelStatus = "Posts("+QString().number(currentBlogPostsCount)+")";
        postModelState = ProgressEntry::Success;
        emit setPostModelStatus(postModelState, postModelStatus);
    }
    else{
        qDebug()<<"Delete Poost"<<tempPostsIdHash.value(post->LocalId())<<post->LocalId();
        postsModel->clear();
        tempPostsIdHash.clear();
    }
}

void Worker::commentRemoved(WPComment comment)
{
    qDebug()<<"Delete Coomment";
    if(commentsModel->rowCount()>0){
        for(int i=0;i<commentsModel->rowCount();i++){
            if(commentsModel->item(i)->data(CommentsEntry::IdRole).toString()==comment->LocalId()){
                qDebug()<<"Delete Coomment"<<i<<comment->LocalId()<<commentsModel->removeRow(i);
                break;
            }
        }
        tempCommentsIdHash.remove(comment->LocalId());

        if(postCommentsModel->rowCount()>0){
            if(postCommentsModel->item(0)->data(CommentsEntry::PostIdRole).toString()==comment->iPostId){
                qDebug()<<"Coomment to be removed from post model";
                for(int i=0;i<postCommentsModel->rowCount();i++){
                    if(postCommentsModel->item(i)->data(CommentsEntry::IdRole).toString()==comment->LocalId()){
                        qDebug()<<"Delete Coomment"<<i<<comment->LocalId()<<postCommentsModel->removeRow(i);
                        break;
                    }
                }
            }
            tempPostCommentsIdHash.remove(comment->LocalId());
        }

//        iBlogs->GetCurrentBlog()->iPosts.RefreshAll();
        WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByItemId(comment->iPostId);
        for(int i=0;i<postsModel->rowCount();i++)
        {
            if(postsModel->item(i)->data(PostsEntry::LocalIdRole) == post->LocalId()){
                postsModel->item(i)->setData(postsModel->item(i)->data(PostsEntry::CommentsCountRole).toInt()-1, PostsEntry::CommentsCountRole);
                break;
            }
        }

        currentBlogCommentsCount = commentsModel->rowCount();
        if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
            blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogCommentsCount, BlogEntry::CommentsCountRole);
            emit updateCurrentBlogCommentsCount(currentBlogCommentsCount);
        }
        commentModelStatus = "Comments("+QString().number(currentBlogCommentsCount)+")";
        commentModelState = ProgressEntry::Success;
        emit setCommentModelStatus(commentModelState, commentModelStatus);
    }
    else{
        commentsModel->clear();
        tempCommentsIdHash.clear();
    }
}

void Worker::commentChanged(WPComment comment)
{
    if(!comment->IsDirty()){
        if(commentsModel->rowCount()>0){
            qDebug()<<"Edit Coomment"<<comment->LocalId();
            if(comment->iStatus == "approve" || comment->iStatus == "hold"){

                for(int i=0;i<commentsModel->rowCount();i++){
                    if(commentsModel->item(i)->data(CommentsEntry::IdRole).toString()==comment->LocalId()){
                        commentsModel->item(i)->setData(comment->LocalId(), CommentsEntry::IdRole);
                        commentsModel->item(i)->setData(comment->iUserId, CommentsEntry::UserIdRole);
                        commentsModel->item(i)->setData(comment->iContent, CommentsEntry::ContentRole);
                        QString preview = comment->iContent;
                        if(preview.length()>64)
                        preview = preview.left(64)+"...";
                        commentsModel->item(i)->setData(preview, CommentsEntry::ContentPreviewRole);
                        commentsModel->item(i)->setData(comment->iPostId, CommentsEntry::PostIdRole);
                        commentsModel->item(i)->setData(comment->iPostTitle, CommentsEntry::PostTitleRole);
                        commentsModel->item(i)->setData(comment->iAuthor, CommentsEntry::AuthorRole);
                        commentsModel->item(i)->setData(comment->iAuthorEmail, CommentsEntry::AuthorEmailRole);
                        commentsModel->item(i)->setData(comment->iAuthorUrl, CommentsEntry::AuthorUrlRole);
                        commentsModel->item(i)->setData(comment->iAuthorIp, CommentsEntry::AuthorIpRole);
                        commentsModel->item(i)->setData(comment->iStatus, CommentsEntry::StatusRole);
                        QByteArray gravatarEmail = comment->iAuthorEmail.trimmed().toUtf8();
                        qDebug()<<"Hassh"<<comment->iAuthorEmail.trimmed().toUtf8()<<QCryptographicHash::hash (gravatarEmail, QCryptographicHash::Md5 ).toHex();
                        commentsModel->item(i)->setData("http://www.gravatar.com/avatar/"+QString(QCryptographicHash::hash (comment->iAuthorEmail.trimmed().toUtf8(), QCryptographicHash::Md5 ).toHex())+"?s=64&d=mm", CommentsEntry::GravatarUrlRole);
                        commentsModel->item(i)->setData(comment->iDateCreated.toString(Qt::DefaultLocaleShortDate), CommentsEntry::DateCreatedRole);
                        commentsModel->item(i)->setData(comment->iDateCreated.toMSecsSinceEpoch(), CommentsEntry::SortDateRole);
                        commentsModel->setSortRole(CommentsEntry::SortDateRole);
                        commentsModel->sort(0, Qt::DescendingOrder);
                        break;
                    }
                }

                if(postCommentsModel->rowCount()>0){
                    if(postCommentsModel->item(0)->data(CommentsEntry::PostIdRole).toString()==comment->iPostId){
                        for(int i=0;i<postCommentsModel->rowCount();i++){
                            if(postCommentsModel->item(i)->data(CommentsEntry::IdRole).toString()==comment->LocalId()){
                                postCommentsModel->item(i)->setData(comment->LocalId(), CommentsEntry::IdRole);
                                postCommentsModel->item(i)->setData(comment->iUserId, CommentsEntry::UserIdRole);
                                postCommentsModel->item(i)->setData(comment->iContent, CommentsEntry::ContentRole);
                                QString preview = comment->iContent;
                                if(preview.length()>64)
                                preview = preview.left(64)+"...";
                                postCommentsModel->item(i)->setData(preview, CommentsEntry::ContentPreviewRole);
                                postCommentsModel->item(i)->setData(comment->iPostId, CommentsEntry::PostIdRole);
                                postCommentsModel->item(i)->setData(comment->iPostTitle, CommentsEntry::PostTitleRole);
                                postCommentsModel->item(i)->setData(comment->iAuthor, CommentsEntry::AuthorRole);
                                postCommentsModel->item(i)->setData(comment->iAuthorEmail, CommentsEntry::AuthorEmailRole);
                                postCommentsModel->item(i)->setData(comment->iAuthorUrl, CommentsEntry::AuthorUrlRole);
                                postCommentsModel->item(i)->setData(comment->iAuthorIp, CommentsEntry::AuthorIpRole);
                                postCommentsModel->item(i)->setData(comment->iStatus, CommentsEntry::StatusRole);
                                QByteArray gravatarEmail = comment->iAuthorEmail.trimmed().toUtf8();
                                qDebug()<<"Hassh"<<comment->iAuthorEmail.trimmed().toUtf8()<<QCryptographicHash::hash (gravatarEmail, QCryptographicHash::Md5 ).toHex();
                                postCommentsModel->item(i)->setData("http://www.gravatar.com/avatar/"+QString(QCryptographicHash::hash (comment->iAuthorEmail.trimmed().toUtf8(), QCryptographicHash::Md5 ).toHex())+"?s=64&d=mm", CommentsEntry::GravatarUrlRole);
                                postCommentsModel->item(i)->setData(comment->iDateCreated.toString(Qt::DefaultLocaleShortDate), CommentsEntry::DateCreatedRole);
                                postCommentsModel->item(i)->setData(comment->iDateCreated.toMSecsSinceEpoch(), CommentsEntry::SortDateRole);
                                postCommentsModel->setSortRole(CommentsEntry::SortDateRole);
                                postCommentsModel->sort(0, Qt::AscendingOrder);
                                break;
                            }
                        }
                    }
                }
            }
            else if(comment->iStatus == "spam")
                commentRemoved(comment);

//            WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByItemId(comment->iPostId);
//            emit iBlogs->GetCurrentBlog()->iPosts.GetItem(post->iLocalId, post->iItemId);
//            for(int i=0;i<postsModel->rowCount();i++)
//            {
//                if(postsModel->item(i)->data(PostsEntry::LocalIdRole) == post->LocalId()){
//                    postsModel->item(i)->setData(post->GetComments().count(), PostsEntry::CommentsCountRole);
//                    break;
//                }
//            }

            currentBlogCommentsCount = commentsModel->rowCount();
            if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
                blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogCommentsCount, BlogEntry::CommentsCountRole);
                emit updateCurrentBlogCommentsCount(currentBlogCommentsCount);
            }
            commentModelStatus = "Comments("+QString().number(currentBlogCommentsCount)+")";
            commentModelState = ProgressEntry::Success;
            emit setCommentModelStatus(commentModelState, commentModelStatus);
        }
        else{
            commentsModel->clear();
            tempCommentsIdHash.clear();
        }
    }
}

void Worker::commentAdded(WPComment comment)
{
    qDebug()<<"New Coomment"<<comment->LocalId();
    if(!tempCommentsIdHash.contains(comment->LocalId())){
        QStandardItem *it = new QStandardItem();
        it->setData(comment->LocalId(), CommentsEntry::IdRole);
        it->setData(comment->iUserId, CommentsEntry::UserIdRole);
        it->setData(comment->iContent, CommentsEntry::ContentRole);
        QString preview = comment->iContent;
        if(preview.length()>64)
            preview = preview.left(64)+"...";
        it->setData(preview, CommentsEntry::ContentPreviewRole);
        it->setData(comment->iPostId, CommentsEntry::PostIdRole);
        it->setData(comment->iPostTitle, CommentsEntry::PostTitleRole);
        it->setData(comment->iAuthor, CommentsEntry::AuthorRole);
        it->setData(comment->iAuthorEmail, CommentsEntry::AuthorEmailRole);
        it->setData(comment->iAuthorUrl, CommentsEntry::AuthorUrlRole);
        it->setData(comment->iAuthorIp, CommentsEntry::AuthorIpRole);
        it->setData(comment->iStatus, CommentsEntry::StatusRole);
        QByteArray gravatarEmail = comment->iAuthorEmail.trimmed().toUtf8();
        qDebug()<<"Hassh"<<comment->iAuthorEmail.trimmed().toUtf8()<<QCryptographicHash::hash (gravatarEmail, QCryptographicHash::Md5 ).toHex();
        it->setData("http://www.gravatar.com/avatar/"+QString(QCryptographicHash::hash (comment->iAuthorEmail.trimmed().toUtf8(), QCryptographicHash::Md5 ).toHex())+"?s=64&d=mm", CommentsEntry::GravatarUrlRole);
        it->setData(comment->iDateCreated.toString(Qt::DefaultLocaleShortDate), CommentsEntry::DateCreatedRole);
        tempCommentsIdHash.insert(comment->LocalId(), commentsModel->rowCount());
        commentsModel->appendRow(it);
        commentsModel->setSortRole(CommentsEntry::SortDateRole);
        commentsModel->sort(0, Qt::DescendingOrder);
        if(postCommentsModel->rowCount()>0 && postCommentsModel->item(0)->data(CommentsEntry::PostIdRole).toString() == comment->iPostId)
        {
            QStandardItem *it = new QStandardItem();
            it->setData(comment->LocalId(), CommentsEntry::IdRole);
            it->setData(comment->iUserId, CommentsEntry::UserIdRole);
            it->setData(comment->iContent, CommentsEntry::ContentRole);
            QString preview = comment->iContent;
            if(preview.length()>64)
            preview = preview.left(64)+"...";
            it->setData(preview, CommentsEntry::ContentPreviewRole);
            it->setData(comment->iPostId, CommentsEntry::PostIdRole);
            it->setData(comment->iPostTitle, CommentsEntry::PostTitleRole);
            it->setData(comment->iAuthor, CommentsEntry::AuthorRole);
            it->setData(comment->iAuthorEmail, CommentsEntry::AuthorEmailRole);
            it->setData(comment->iAuthorUrl, CommentsEntry::AuthorUrlRole);
            it->setData(comment->iAuthorIp, CommentsEntry::AuthorIpRole);
            it->setData(comment->iStatus, CommentsEntry::StatusRole);
            QByteArray gravatarEmail = comment->iAuthorEmail.trimmed().toUtf8();
            qDebug()<<"Hassh"<<comment->iAuthorEmail.trimmed().toUtf8()<<QCryptographicHash::hash (gravatarEmail, QCryptographicHash::Md5 ).toHex();
            it->setData("http://www.gravatar.com/avatar/"+QString(QCryptographicHash::hash (comment->iAuthorEmail.trimmed().toUtf8(), QCryptographicHash::Md5 ).toHex())+"?s=64&d=mm", CommentsEntry::GravatarUrlRole);
            it->setData(comment->iDateCreated.toString(Qt::DefaultLocaleShortDate), CommentsEntry::DateCreatedRole);
            it->setData(comment->iDateCreated.toMSecsSinceEpoch(), CommentsEntry::SortDateRole);
            tempPostCommentsIdHash.insert(comment->LocalId(), postCommentsModel->rowCount());
            postCommentsModel->appendRow(it);
            postCommentsModel->setSortRole(CommentsEntry::SortDateRole);
            postCommentsModel->sort(0, Qt::AscendingOrder);
        }
    }

    WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByItemId(comment->iPostId);
    for(int i=0;i<postsModel->rowCount();i++)
    {
        if(postsModel->item(i)->data(PostsEntry::LocalIdRole) == post->LocalId()){
            postsModel->item(i)->setData(post->GetComments().count(), PostsEntry::CommentsCountRole);
            break;
        }
    }

    currentBlogCommentsCount = commentsModel->rowCount();
    if(blogsModel->rowCount()!=0&&!tempBlogIdHash.isEmpty()){
        blogsModel->item(tempBlogIdHash.value(iBlogs->GetCurrentBlog()->LocalId()))->setData(currentBlogCommentsCount, BlogEntry::CommentsCountRole);
        emit updateCurrentBlogCommentsCount(currentBlogCommentsCount);
    }
}

void Worker::categoryAdded(WPDataCategory *category)
{
    qDebug()<<"Category Added>>>>>>>>>>>>>>>>>>>>>"<<category->iCategoryName<<category->iCategoryDescription<<category->iDescription;
    QStandardItem* it = new QStandardItem();
    it->setData(category->iCategoryId, CategoryEntry::CategoryIdRole);
    it->setData(category->iCategoryName, CategoryEntry::CategoryNameRole);
    it->setData(category->iCategoryDescription, CategoryEntry::DescriptionRole);
    it->setData(false, CategoryEntry::MarkedRole);
    categoriesModel->appendRow(it);

    categoriesModel->setSortRole(CategoryEntry::CategoryNameRole);
    categoriesModel->sort(0, Qt::AscendingOrder);
    this->markCategory(category->iCategoryName);
}

void Worker::preparePostCommentsModel(QString id)
{
    postCommentsModel->clear();
    tempPostCommentsIdHash.clear();
    WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(id);
    WPComment comment;
    for(int i=0;i<post->GetComments().count();i++){
        comment = post->GetComments().at(i);
        QStandardItem* it = new QStandardItem();
        it->setData(comment->LocalId(), CommentsEntry::IdRole);
        it->setData(comment->iUserId, CommentsEntry::UserIdRole);
        it->setData(comment->iContent, CommentsEntry::ContentRole);
        QString preview = comment->iContent;
        if(preview.length()>64)
            preview = preview.left(64)+"...";
        it->setData(preview, CommentsEntry::ContentPreviewRole);
        it->setData(comment->iPostId, CommentsEntry::PostIdRole);
        it->setData(comment->iPostTitle, CommentsEntry::PostTitleRole);
        it->setData(comment->iAuthor, CommentsEntry::AuthorRole);
        it->setData(comment->iAuthorEmail, CommentsEntry::AuthorEmailRole);
        it->setData(comment->iAuthorUrl, CommentsEntry::AuthorUrlRole);
        it->setData(comment->iAuthorIp, CommentsEntry::AuthorIpRole);
        it->setData(comment->iStatus, CommentsEntry::StatusRole);
        QByteArray gravatarEmail = comment->iAuthorEmail.trimmed().toUtf8();
        qDebug()<<"Hassh"<<comment->iAuthorEmail.trimmed().toUtf8()<<QCryptographicHash::hash (gravatarEmail, QCryptographicHash::Md5 ).toHex();
        it->setData("http://www.gravatar.com/avatar/"+QString(QCryptographicHash::hash (comment->iAuthorEmail.trimmed().toUtf8(), QCryptographicHash::Md5 ).toHex())+"?s=64&d=mm", CommentsEntry::GravatarUrlRole);
        it->setData(comment->iDateCreated.toString(Qt::DefaultLocaleShortDate), CommentsEntry::DateCreatedRole);
        it->setData(comment->iDateCreated.toMSecsSinceEpoch(), CommentsEntry::SortDateRole);
        tempPostCommentsIdHash.insert(comment->LocalId(), postCommentsModel->rowCount());
        postCommentsModel->appendRow(it);
    }
    postCommentsModel->setSortRole(CommentsEntry::SortDateRole);
    postCommentsModel->sort(0, Qt::AscendingOrder);
    qDebug()<<"Prepared Comments Model"<<postCommentsModel->rowCount();

}

void Worker::searchMedia(QString type)
{
    qDebug()<<"Search Media";
    searchThread = new MediaSearchThread(this);
    qRegisterMetaType<QFileInfo>("QFileInfo");
    connect(searchThread, SIGNAL(finished()), this, SLOT(searchFinished()));

    if(type == "image")
        searchThread->setSearchType(MediaSearchThread::SearchImages);
    else
        searchThread->setSearchType(MediaSearchThread::SearchVideos);
    searchMediaState = ProgressEntry::Processing;
    searchMediaStatus = "Searching media items...";
    emit setSearchMediaStatus(searchMediaState, searchMediaStatus);
    uiThread->setPriority(QThread::HighestPriority);
    searchThread->start(QThread::LowPriority);
}

void Worker::searchFinished()
{
    uiThread->setPriority(QThread::NormalPriority);
    if(!isSearchCanceled()) {
        prepareLocalMediaModel();
    }
    searchMediaState = ProgressEntry::Success;
    searchMediaStatus = "Search finished!";
    emit setSearchMediaStatus(searchMediaState, searchMediaStatus);

    addMediaState = ProgressEntry::None;
    addMediaStatus = "";
    emit setAddMediaStatus(addMediaState, addMediaStatus);

    delete searchThread;
    searchThread = NULL;
}

void Worker::prepareLocalMediaModel()
{
    qDebug()<<"Preparing Media Model";
    QString thumb;
    uiThread->setPriority(QThread::HighestPriority);
    this->thread()->setPriority(QThread::LowPriority);
    localMediaModel->clear();
    QFileInfoList const & list = searchThread->InfoList();
    qDebug()<<"Items found"<<list.count();
    int tempCount = 0;
    for(int i=0;i<list.count();i++) {
        //qDebug()<<"File"<<list.at(i).fileName()<<list.at(i).filePath()<<list.at(i).size()/1024*1024;
        QStandardItem *it = new QStandardItem();
        it->setData(list.at(i).fileName(), LocalMediaEntry::FileNameRole);

        it->setData(list.at(i).filePath(), LocalMediaEntry::FilePathRole);
        QString type = list.at(i).suffix();
        if(type=="jpg"||type=="jpeg"||type=="png"||type=="bmp")
            type = "image/"+type;
        else if(type=="mp4"||type=="flv"||type=="avi"||type=="mpeg"||type=="mpg")
            type = "video/"+type;
        else
            type = "file/"+type;
        it->setData(type, LocalMediaEntry::FileTypeRole);
        it->setData(QString::number(float(list.at(i).size())/(1024*1024),'g', 2), LocalMediaEntry::FileSizeRole);

        if(list.at(i).suffix()=="jpg"||list.at(i).suffix()=="jpeg"||list.at(i).suffix()=="png"||list.at(i).suffix()=="bmp") {
            qDebug()<<"It is a image!";
            imgReader->setFileName(list.at(i).absoluteFilePath());
            int rotation;
            if(QExifImageHeader(list.at(i).absoluteFilePath()).value(QExifImageHeader::Orientation).toShort() == 6)
                rotation = 90;
            else if(QExifImageHeader(list.at(i).absoluteFilePath()).value(QExifImageHeader::Orientation).toShort() == 3)
                rotation = 180;
            else if(QExifImageHeader(list.at(i).absoluteFilePath()).value(QExifImageHeader::Orientation).toShort() == 8)
                rotation = 270;
            else
                rotation = 0;

            QDir mkd = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
            //qDebug()<<"QDesktopServices::DataLocation"<<mkd.path();
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
            thumb = QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache/"+
                    QCryptographicHash::hash(list.at(i).absoluteFilePath().toUtf8(), QCryptographicHash::Md5).toHex()+".jpg";
            if(!QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache").exists())
                mkd.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache");
#else
            thumb = QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache/"+
                    QCryptographicHash::hash(list.at(i).absoluteFilePath().toUtf8(), QCryptographicHash::Md5).toHex()+".jpg";
            if(!QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache").exists())
                mkd.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache");
#endif
            qDebug()<<thumb<<QFile::exists(thumb);
            if(!QFile::exists(thumb)){
                qDebug()<<"Create thumbnail!";
                imgReader->setFileName(list.at(i).absoluteFilePath());
                imgReader->setScaledSize(QSize(128,128));
                QTransform rot;
                rot.rotate(rotation);
                QImage temp = imgReader->read().transformed(rot, Qt::FastTransformation);
                imgWriter->setFileName(thumb);
                imgWriter->setFormat("jpg");
                qDebug()<<"write"<<thumb<<imgWriter->write(temp)<<imgWriter->errorString();
            }
        }
        it->setData(QUrl::fromLocalFile(thumb) , LocalMediaEntry::FileThumbRole);
//#if !defined(Q_OS_SYMBIAN)&& !defined(Q_WS_MAEMO_5) && !defined(QT_SIMULATOR) && !defined(Q_WS_WIN)
//        QString thumb = "file:///home/user/.thumbnails/grid/" +
//                        QCryptographicHash::hash(QUrl::toPercentEncoding(QUrl::fromLocalFile(list.at(i).filePath()).toString(), QUrl::fromLocalFile(list.at(i).filePath()).toEncoded()," "),QCryptographicHash::Md5).toHex() +
//                        ".jpeg";
//        it->setData(thumb , LocalMediaEntry::FileThumbRole);
//#endif
        localMediaModel->appendRow(it);

        // Process events after every 10 items to keep ui responsive
        if(++tempCount == 10) {
            tempCount = 0;
            QApplication::processEvents();

            // this might be cancelled during processEvents();
            if(isSearchCanceled()) {
                return;
            }
        }
    }
    uiThread->setPriority(QThread::NormalPriority);
    this->thread()->setPriority(QThread::NormalPriority);
}

void Worker::addFile(QString file, QString type)
{
    qDebug()<<"Upload request"<<file;
    iUploadFile = new WPMediaFile(file);
    if(type.contains("image")){
        iUploadFile->type = WPMediaFile::MEDIA_TYPE_IMAGE;
        iUploadFile->rotationAngle = 0;
        iUploadFile->size = QSize(0,0);
        iUploadFile->alignment = WPMediaFile::Center;
    }else if(type.contains("video"))
        iUploadFile->type = WPMediaFile::MEDIA_TYPE_IMAGE;
    else
        iUploadFile->type = WPMediaFile::MEDIA_TYPE_NONE;

    emit this->uploadFile(iUploadFile);
}

void Worker::fileUploaded(WPMediaFile file)
{
    qDebug()<<"File uploaded"<<file.filename<<file.type<<file.resultUrl<<file.getHtmlSnippet();
    iUploadFile->resultUrl = file.resultUrl;
    /*QString thumb;
    if(file.type==WPMediaFile::MEDIA_TYPE_IMAGE)
        thumb = file.resultUrl.insert(file.resultUrl.length()-4, "-150x150");
    else
        thumb = file.resultUrl.replace(file.serverFilename, "document.png");*/
    emit this->addFileSuccessful(file.resultUrl,
                                 file.serverFilename.split("/").last(),
                                 file.getMimeType(),
                                 "image/"+QFileInfo(file.filename).suffix(),
                                 QFileInfo(file.filename).fileName(),
                                 file.resultUrl,
                                 file.getHtmlSnippet());
    this->refreshCurrentBlogMedia();
    delete iUploadFile;
    iUploadFile = NULL;
}

void Worker::fileUploadFailed(WPMediaFile file)
{
    qDebug()<<"File upload failed"<<file.filename;
}

void Worker::markCurrentPostCategories(QString id)
{
    WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(id);
    for(int i=0;i<categoriesModel->rowCount();i++){
        if(post->iCategories.contains(categoriesModel->item(i)->data(CategoryEntry::CategoryNameRole).toString(), Qt::CaseSensitive))
            categoriesModel->item(i)->setData(true, CategoryEntry::MarkedRole);
        else
            categoriesModel->item(i)->setData(false, CategoryEntry::MarkedRole);
    }
}

void Worker::markCategory(QString cat)
{
    for(int i=0;i<categoriesModel->rowCount();i++){
        if(categoriesModel->item(i)->data(CategoryEntry::CategoryNameRole).toString() == cat){
            categoriesModel->item(i)->setData(!categoriesModel->item(i)->data(CategoryEntry::MarkedRole).toBool(),CategoryEntry::MarkedRole);
            break;
        }
    }
}

void Worker::addNewCategory(QString cat)
{
    qDebug()<<"Worker->>>>>>Add Category";
    iBlogs->GetCurrentBlog()->AddCategory(cat);
}

void Worker::setCategoriesToPost(QString id)
{
    //WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(id);
    qDebug()<<"Setting Cat";
    QStringList categories;
    for(int i=0;i<categoriesModel->rowCount();i++){
        if(categoriesModel->item(i)->data(CategoryEntry::MarkedRole).toBool()){
            categories.append(categoriesModel->item(i)->data(CategoryEntry::CategoryNameRole).toString());
        }
    }
    //post->iCategories = categories;
    emit updateCurrentPostCategories(categories.join(", "));
}

void Worker::unmarkAllCategories()
{
    for(int i=0;i<categoriesModel->rowCount();i++){
        categoriesModel->item(i)->setData(false, CategoryEntry::MarkedRole);
    }
}

void Worker::editPost(QString id,
                      QString title,
                      QString content,
                      QString tags,
                      QString date,
                      QString password,
                      QString status)
{
    WPPost post = iBlogs->GetCurrentBlog()->iPosts.GetByLocalId(id);
    QDateTime d = QDateTime::fromString(date, "yyyy/M/d h:m:s");
    qDebug()<<date<<d<<d.toString(Qt::SystemLocaleShortDate);
    QStringList cat;
    for(int i=0;i<categoriesModel->rowCount();i++)
    {
        if(categoriesModel->item(i)->data(CategoryEntry::MarkedRole).toBool())
            cat.append(categoriesModel->item(i)->data(CategoryEntry::CategoryNameRole).toString());
    }
    if(status == "local draft"){
        qDebug()<<"Local Draft"<<title;
        post->UpdateLocal(title,
                          content,
                          tags,
                          d,
                          password,
                          cat);
    }else{
        post->UpdateStatus(title,
                           content,
                           tags,
                           d,
                           password,
                           cat,
                           status.toLower(),
                           QList<WPMediaFile>());
    }
}

void Worker::addNewPost(QString title,
                        QString content,
                        QString tags,
                        QString date,
                        QString password,
                        QString status)
{
    WPPost post;
    post = new WPDataPost(*iBlogs->GetCurrentBlog());
    QDateTime d = QDateTime::fromString(date, "yyyy/M/d h:m:s");
    qDebug()<<date<<d<<d.toString(Qt::SystemLocaleShortDate);
    QStringList cat;
    for(int i=0;i<categoriesModel->rowCount();i++)
    {
        if(categoriesModel->item(i)->data(CategoryEntry::MarkedRole).toBool())
            cat.append(categoriesModel->item(i)->data(CategoryEntry::CategoryNameRole).toString());
    }
    if(status == "local draft")
    {
        qDebug()<<"Local Draft"<<title;
        post->UpdateLocal(title,
                          content,
                          tags,
                          d,
                          password,
                          cat);
    }
    else
    {
        post->UpdateStatus(title,
                           content,
                           tags,
                           d,
                           password,
                           cat,
                           status,
                           QList<WPMediaFile>());
        postPub = post->LocalId();
    }
}

void Worker::editPage(QString id,
                      QString title,
                      QString content,
                      QString password,
                      QString date,
                      QString status)
{
    WPPage page = iBlogs->GetCurrentBlog()->iPages.GetByLocalId(id);
    QDateTime d = QDateTime::fromString(date, "yyyy/M/d h:m:s");
    qDebug()<<date<<d<<d.toString(Qt::SystemLocaleShortDate);
    if(status.toLower() == "local draft"){
        qDebug()<<"Local Draft"<<title;
        page->UpdateLocal(title,
                          content,
                          password,
                          d);
    }else{
        page->UpdateStatus(title,
                           content,
                           password,
                           d,
                           status.toLower(),
                           QList<WPMediaFile>());
    }
}

void Worker::addNewPage(QString title,
                        QString content,
                        QString password,
                        QString date,
                        QString status)
{
    WPPage page;
    page = new WPDataPage(*iBlogs->GetCurrentBlog());
    QDateTime d = QDateTime::fromString(date, "yyyy/M/d h:m:s");
    qDebug()<<date<<d<<d.toString(Qt::SystemLocaleShortDate);
    if(status.toLower() == "local draft")
    {
        qDebug()<<"Local Draft"<<title;
        page->UpdateLocal(title,
                          content,
                          password,
                          d);
    }
    else
    {
        page->UpdateStatus(title,
                           content,
                           password,
                           d,
                           status,
                           QList<WPMediaFile>());
        pagePub = page->LocalId();
    }
}

void Worker::checkSettings()
{
    QSettings settings("ShowStopper","CutePress");
    if(!settings.value("uiTheme").toString().isEmpty() && settings.value("uiTheme").toString()=="light"){
        uiTheme = "light";
    }
    else
    {
        uiTheme = "dark";
        settings.setValue("uiTheme", uiTheme);
    }
    qDebug()<<"Theme"<<uiTheme;
    //emit updateSettings(uiTheme);
}

void Worker::refreshThumbnailCache()
{
    qDebug()<<"Deleting thumbnail cache";
    QDir cacheDir;
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    cacheDir = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/cache/");
#else
    cacheDir = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/.cutepress/_PAlbTN/cache/");
#endif
    QFileInfoList entries = cacheDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    int count = entries.size();
    for(int i=0;i<count;i++) {
        qDebug()<<entries.at(i).fileName()<<cacheDir.remove(entries.at(i).fileName());
    }
}

void Worker::saveSettings(QString theme)
{
    QSettings settings("ShowStopper","CutePress");
    if(theme!=uiTheme) {
        uiTheme = theme;
        settings.setValue("uiTheme", uiTheme);
        qDebug()<<"theme saved";
    }
}
