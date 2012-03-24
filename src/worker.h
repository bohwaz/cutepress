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

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDebug>
#include <QPointer>
#include <QImageWriter>
#include <QImageReader>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QSettings>

#include "WPData.h"
#include "roleitemmodel.h"
#include "md5.h"
#include "MediaSearchThread.h"
#include "thumbnailthread.h"
#include "qexifimageheader.h"

/**
 * @struct ProgressEntry
 * Structure for defining roles for recentle uploaded model
 */
struct ProgressEntry {

    enum ProgressRoles {
        None,
        Success,
        Processing,
        Error
    };
};

/**
 * @struct BlogEntry
 * Structure for defining roles for blogs model
 */
struct BlogEntry {

    enum BlogRoles {
        TitleRole = Qt::UserRole + 1,
        IdRole,
        UrlRole,
        UsernameRole,
        PasswordRole,
        IsAdminRole,
        IsWordpressRole,
        PagesCountRole,
        PostsCountRole,
        CommentsCountRole
    };
};

/**
 * @struct MediaDirEntry
 * Structure for defining roles for directory model
 */
struct MediaDirEntry {

    enum MediaDirRoles {
        PathRole = Qt::UserRole + 1
    };
};


/**
 * @struct PagesEntry
 * Structure for defining roles for pages model
 */
struct PagesEntry {

    enum PagesRoles {
        TitleRole = Qt::UserRole + 1,
        LocalIdRole,
        DescRole,
        LinkRole,
        PermaLinkRole,
        WpPageParentTitleRole,
        WpAuthorDisplayNameRole,
        WpAuthorRole,
        StatusRole,
        PasswordRole,
        DateCreatedRole,
        PageTimeRole,
        PageDateRole,
        SortDateRole
    };
};

/**
 * @struct PostsEntry
 * Structure for defining roles for posts model
 */
struct PostsEntry {

    enum PostsRoles {
        TitleRole = Qt::UserRole + 1,
        LocalIdRole,
        DescRole,
        LinkRole,
        PermaLinkRole,
        WpAuthorDisplayNameRole,
        CategoriesRole,
        CommentsCountRole,
        StatusRole,
        PasswordRole,
        DateCreatedRole,
        PostTimeRole,
        PostDateRole,
        SortDateRole
    };
};

/**
 * @struct PostStatus
 * Structure for defining roles for post status
 */
struct PostStatus {

    enum PostsRoles {
        Orphan = Qt::UserRole + 1,
        Published,
        Private,
        Pending,
        Future,
        Draft,
        LocalDraft,
        Busy,
        Trash
    };
};

/**
 * @struct CommentsEntry
 * Structure for defining roles for comments model
 */
struct CommentsEntry {

    enum CommentsRoles {
        IdRole = Qt::UserRole + 1,
        UserIdRole,
        ContentRole,
        ContentPreviewRole,
        PostIdRole,
        PostTitleRole,
        AuthorRole,
        AuthorUrlRole,
        AuthorEmailRole,
        AuthorIpRole,
        StatusRole,
        GravatarUrlRole,
        DateCreatedRole,
        SortDateRole
    };
};

/**
 * @struct CategoryEntry
 * Structure for defining roles for categories model
 */
struct CategoryEntry {

    enum CategoryRoles {
        CategoryIdRole = Qt::UserRole + 1,
        ParentIdRole,
        DescriptionRole,
        CategoryNameRole,
        HtmlUrlRole,
        MarkedRole
    };
};

/**
 * @struct MediaEntry
 * Structure for defining roles for media items model
 */
struct MediaEntry {

    enum MediaRoles {
        TitleRole = Qt::UserRole,
        ParentRole,
        DescriptionRole,
        IsThumbnailRole,
        CaptionRole,
        LinkRole,
        MetaDataRole,
        FileTypeRole,
        DateCreatedRole,
        SortDateRole
    };
};

/**
 * @struct LocalMediaEntry
 * Structure for defining roles for local media items model
 */
struct LocalMediaEntry {

    enum LocalMediaRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        FileSizeRole,
        FileThumbRole,
        FileTypeRole
    };
};


class Worker : public QObject
{
    Q_OBJECT



public:
    Worker();
    ~Worker();
    void checkSettings();
    RoleItemModel *blogsModel;
    RoleItemModel *pagesModel;
    RoleItemModel *postsModel;
    RoleItemModel *postCommentsModel;
    RoleItemModel *commentsModel;
    RoleItemModel *categoriesModel;
    RoleItemModel *mediaModel;
    RoleItemModel *localMediaModel;
    RoleItemModel *mediaDirModel;
    bool isBlogFound;
    int currentBlogPagesCount;
    int currentBlogPostsCount;
    int currentBlogCommentsCount;
    int currentBlogMediaCount;
    QString currentBlogName;
    bool isCurrentBlogWPHosted;
    QString pageModelStatus;
    int pageModelState;
    QString postModelStatus;
    int postModelState;
    QString commentModelStatus;
    int commentModelState;
    QString mediaModelStatus;
    int mediaModelState;
    QString addPostStatus;
    int addPostState;
    QString addPageStatus;
    int addPageState;
    QString addCommentStatus;
    int addCommentState;
    QString addMediaStatus;
    int addMediaState;
    QString addCategoryStatus;
    int addCategoryState;
    QString searchMediaStatus;
    int searchMediaState;
    QThread *uiThread;
    QString uiTheme;
    QString uiIconType;

signals:
    void blogFound(QVariant);
    void getUserBlogUnsuccessful(QVariant);
    void getUserBlogSuccessful();
    void updateCurrentBlogDetails(QVariant,QVariant);
    void updateCurrentBlogPagesCount(QVariant);
    void updateCurrentBlogPostsCount(QVariant);
    void updateCurrentBlogCommentsCount(QVariant);
    void updateCurrentBlogMediaCount(QVariant);
    void showPost(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant);
    void setPageModelStatus(QVariant,QVariant);
    void setPostModelStatus(QVariant,QVariant);
    void setCommentModelStatus(QVariant,QVariant);
    void setMediaModelStatus(QVariant,QVariant);
    void setAddPostStatus(QVariant,QVariant);
    void setAddPageStatus(QVariant,QVariant);
    void setAddCommentStatus(QVariant,QVariant);
    void setAddMediaStatus(QVariant,QVariant);
    void setAddCategoryStatus(QVariant,QVariant);
    void setSearchMediaStatus(QVariant,QVariant);
    void uploadFile(WPMediaFile *);
    void addFileSuccessful(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant);
    void updateCurrentPostCategories(QVariant);
    void newPagePublished();
    void newPagePublishFailed(QVariant);
    void newPostPublished();
    void newPostPublishFailed(QVariant);
    /**
     * Emitted for setting selected directory from selection dialog in QML UI
     * @param dir - selected directory
     */
    void setSelectedDir(QVariant);
    void updateSettings(QVariant, QVariant);

public slots:
    void addNewBlog(QString blogUrl,
                    QString username,
                    QString password,
                    bool resizePhotosCheck);
    void editBlog(QString id,
                  QString blogUrl,
                  QString username,
                  QString password,
                  bool resizePhotosCheck);
    void checkBlog();
    void refreshCurrentBlog();
    void openBlog(QString);
    void deleteBlog(QString);
    void editPage(QString,QString,QString,QString,QString,QString);
    void addNewPage(QString,QString,QString,QString,QString);
    void refreshCurrentBlogPages();
    void deletePage(QString);
    void editPost(QString,QString,QString,QString,QString,QString,QString);
    void addNewPost(QString,QString,QString,QString,QString,QString);
    void refreshCurrentBlogPosts();
    void openPost(QString);
    void deletePost(QString);
    void replyToComment(QString,QString);
    void approveComment(QString);
    void unApproveComment(QString);
    void deleteComment(QString);
    void spamComment(QString);
    void addComment(QString,QString);
    void refreshCurrentBlogComments();
    void refreshCurrentBlogMedia();
    void searchMedia(QString);
    void addFile(QString,QString);
    void markCurrentPostCategories(QString);
    void markCategory(QString);
    void setCategoriesToPost(QString);
    void unmarkAllCategories();
    void addNewCategory(QString);
    //void addNewPost();    
    void writeMediaItemsToModel(const int startIdx);
    void refreshThumbnailCache();
    void saveSettings(QString, QString);
    /**
     * Slot for adding new directory in feed directory model from QML UI
     * @param dir - path of new directory
     */
    void addNewDir(QString dir);
    /**
     * Slot for removing existing idx directory from feed directory model from QML UI
     * @param idx - index from feed dir model
     */
    void removeExistingDir(int idx);
    void getDirSelectionDialog();

private slots:
    void updatePageData(const WPDataBlog &);
    void updatePostsData(const WPDataBlog &);
    void updateCommentsData(const WPDataBlog &);
    void updateCategoryData(const WPDataBlog &);
    void updateMediaData(const WPDataBlog &);
    void updateBlogs(const WPDataUsersBlogs &);
    void taskStarted(TWPNetworkEngineCommand);
    void taskFinished(TWPNetworkEngineCommand);
    void taskFailed (TWPNetworkEngineCommand command, QString msg);
    void blogDeleted(int);
    void pageAdded(WPPage);
    void pageChanged(WPPage);
    void pageRemoved(WPPage);
    void postAdded(WPPost);
    void postChanged(WPPost);
    void postRemoved(WPPost);
    void commentAdded(WPComment);
    void commentChanged(WPComment);
    void commentRemoved(WPComment);
    void categoryAdded(WPDataCategory *);
    void searchFinished();
    void thumbnailsCreated();
    void fileUploaded(WPMediaFile);
    void fileUploadFailed(WPMediaFile);

private:
    void preparePostCommentsModel(QString);
    void prepareLocalMediaModel();
    void writeItem(const int i);

    inline bool isSearchCanceled() { return !searchThread || searchThread->IsCanceled(); }
    WPNetworkEngine* iNetworkEngine;
    WPDataUsersBlogs* iBlogs;
    WPDataGravatars* iGravatars;
    QPointer<WPDataBlog> iCurrentBlog;
    QPointer<WPDataBlog> iCurrentBlogPages;
    QPointer<WPDataBlog> iCurrentBlogPosts;
    QPointer<WPDataBlog> iCurrentBlogComments;
    QHash<QString, int> tempBlogIdHash;
    QHash<QString, int> tempPagesIdHash;
    QHash<QString, int> tempPostsIdHash;
    QHash<QString, int> tempCommentsIdHash;
    QHash<QString, int> tempPostCommentsIdHash;
    MediaSearchThread *searchThread;
    ThumbnailThread *thumbnailThread;

    WPMediaFile *iUploadFile;
    QString postPub;
    QString pagePub;
    QImageReader *imgReader;
    QImageWriter *imgWriter;
    QStringList mediaDirs;
    bool isMediaDirsChanged;
    QFileInfoList files;
};
#endif
// WORKER_H
