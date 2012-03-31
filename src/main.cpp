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

//![0]
#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QtDeclarative>
#include <QNetworkProxy>
#include <QSplashScreen>
#include <QPixmap>
#include "worker.h"
#include "networkaccessmanagerfactory.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_S60DisablePartialScreenInputMode, false);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name().toLower());
    app.installTranslator(&qtTranslator);
    QTranslator myappTranslator;
    qDebug()<<QLocale::system().name();
    if(myappTranslator.load(":/cutepress_" + QLocale::system().name().toLower()))
        app.installTranslator(&myappTranslator);
    else
        qDebug()<<"File not found";

    int myW = app.desktop()->width();
    int myH = app.desktop()->height();
    QPixmap pixmap(":qml/images/splash-"+QString::number(myW)+"x"+QString::number(myH)+".png");
    if(pixmap.isNull())
        pixmap = QPixmap(":qml/images/splash-360x640.png");

    Qt::WidgetAttribute attribute;
    attribute = Qt::WA_LockPortraitOrientation;
    QSplashScreen splash(pixmap);
    splash.setAttribute(attribute, true);

#ifdef Q_OS_WIN
    splash.show();
#else
    splash.showFullScreen();
#endif

#if defined(Q_OS_SYMBIAN)|| defined(Q_WS_WIN)
    splash.showMessage("Initializing...", Qt::AlignHCenter|Qt::AlignBottom, Qt::black);
#endif

    app.processEvents();
    QDeclarativeView view;
    Worker worker;
    worker.uiThread = view.thread();
    NetworkAccessManagerFactory factory;
    view.engine()->setNetworkAccessManagerFactory(&factory);

#if !defined(Q_OS_SYMBIAN)&& !defined(Q_WS_MAEMO_5) && !defined(QT_SIMULATOR) && !defined(Q_WS_WIN)
    view.setSource(QUrl("qrc:/qml/meego/cutepress.qml"));
    qDebug()<<"Hello";
#else
    view.setSource(QUrl("qrc:/qml/symbian/cutepress.qml"));
#endif

    /*Check for network connection
    QUrl proxyUrl(qgetenv("http_proxy"));
    if (proxyUrl.isValid() && !proxyUrl.host().isEmpty()) {
        int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort);
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else {
        QNetworkProxyQuery npq(QUrl(QLatin1String("http://www.flickr.com")));
        QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
        if (listOfProxies.size() > 0 && listOfProxies[0].type() != QNetworkProxy::NoProxy)
            QNetworkProxy::setApplicationProxy(listOfProxies[0]);
    }*/


    QObject *object = view.rootObject();
    QObject::connect(object, SIGNAL(addNewBlog(QString,QString,QString,bool,int)),
                     &worker, SLOT(addNewBlog(QString,QString,QString,bool,int)));
    QObject::connect(object, SIGNAL(editBlog(QString,QString,QString,QString,bool,int)),
                     &worker, SLOT(editBlog(QString,QString,QString,QString,bool,int)));
    QObject::connect(object, SIGNAL(addNewPage(QString,QString,QString,QString,QString)),
                     &worker, SLOT(addNewPage(QString,QString,QString,QString,QString)));
    QObject::connect(object, SIGNAL(editPage(QString,QString,QString,QString,QString,QString)),
                     &worker, SLOT(editPage(QString,QString,QString,QString,QString,QString)));
    QObject::connect(object, SIGNAL(addNewPost(QString,QString,QString,QString,QString,QString)),
                     &worker, SLOT(addNewPost(QString,QString,QString,QString,QString,QString)));
    QObject::connect(object, SIGNAL(editPost(QString,QString,QString,QString,QString,QString,QString)),
                     &worker, SLOT(editPost(QString,QString,QString,QString,QString,QString,QString)));
    QObject::connect(object, SIGNAL(addNewCategory(QString)),
                     &worker, SLOT(addNewCategory(QString)));
    QObject::connect(object, SIGNAL(uiReady()), &worker, SLOT(checkBlog()));
    QObject::connect(object, SIGNAL(openBlog(QString)), &worker, SLOT(openBlog(QString)));
    QObject::connect(object, SIGNAL(refreshCurrentBlog()), &worker, SLOT(refreshCurrentBlog()));
    QObject::connect(object, SIGNAL(refreshCurrentBlogPages()), &worker, SLOT(refreshCurrentBlogPages()));
    QObject::connect(object, SIGNAL(refreshCurrentBlogPosts()), &worker, SLOT(refreshCurrentBlogPosts()));
    QObject::connect(object, SIGNAL(refreshCurrentBlogComments()), &worker, SLOT(refreshCurrentBlogComments()));
    QObject::connect(object, SIGNAL(refreshCurrentBlogMedia()), &worker, SLOT(refreshCurrentBlogMedia()));
    QObject::connect(object, SIGNAL(openPost(QString)), &worker, SLOT(openPost(QString)));
    QObject::connect(object, SIGNAL(replyToComment(QString,QString)), &worker, SLOT(replyToComment(QString,QString)));
    QObject::connect(object, SIGNAL(deleteBlog(QString)), &worker, SLOT(deleteBlog(QString)));
    QObject::connect(object, SIGNAL(deletePage(QString)), &worker, SLOT(deletePage(QString)));
    QObject::connect(object, SIGNAL(deletePost(QString)), &worker, SLOT(deletePost(QString)));
    QObject::connect(object, SIGNAL(approveComment(QString)), &worker, SLOT(approveComment(QString)));
    QObject::connect(object, SIGNAL(unApproveComment(QString)), &worker, SLOT(unApproveComment(QString)));
    QObject::connect(object, SIGNAL(deleteComment(QString)), &worker, SLOT(deleteComment(QString)));
    QObject::connect(object, SIGNAL(spamComment(QString)), &worker, SLOT(spamComment(QString)));
    QObject::connect(object, SIGNAL(addComment(QString,QString)), &worker, SLOT(addComment(QString,QString)));
    QObject::connect(object, SIGNAL(searchMedia(QString)), &worker, SLOT(searchMedia(QString)));
    QObject::connect(object, SIGNAL(writeMediaItemsToModel(int)), &worker, SLOT(writeMediaItemsToModel(int)));
    QObject::connect(object, SIGNAL(addFile(QString,QString)), &worker, SLOT(addFile(QString,QString)));
    QObject::connect(object, SIGNAL(markCurrentPostCategories(QString)), &worker, SLOT(markCurrentPostCategories(QString)));
    QObject::connect(object, SIGNAL(markCategory(QString)), &worker, SLOT(markCategory(QString)));
    QObject::connect(object, SIGNAL(setCategoriesToPost(QString)), &worker, SLOT(setCategoriesToPost(QString)));
    QObject::connect(object, SIGNAL(unmarkAllCategories()), &worker, SLOT(unmarkAllCategories()));


    QObject::connect(&worker, SIGNAL(blogFound(QVariant)), object, SLOT(blogFound(QVariant)));
    QObject::connect(&worker, SIGNAL(updateCurrentBlogDetails(QVariant,QVariant)), object, SLOT(updateCurrentBlogDetails(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(updateCurrentBlogPagesCount(QVariant)), object, SLOT(updateCurrentBlogPagesCount(QVariant)));
    QObject::connect(&worker, SIGNAL(updateCurrentBlogPostsCount(QVariant)), object, SLOT(updateCurrentBlogPostsCount(QVariant)));
    QObject::connect(&worker, SIGNAL(updateCurrentBlogCommentsCount(QVariant)), object, SLOT(updateCurrentBlogCommentsCount(QVariant)));
    QObject::connect(&worker, SIGNAL(updateCurrentBlogMediaCount(QVariant)), object, SLOT(updateCurrentBlogMediaCount(QVariant)));
    QObject::connect(&worker, SIGNAL(getUserBlogUnsuccessful(QVariant)), object, SLOT(getUserBlogUnsuccessful(QVariant)));
    QObject::connect(&worker, SIGNAL(getUserBlogSuccessful()), object, SLOT(getUserBlogSuccessful()));
    QObject::connect(&worker, SIGNAL(newPagePublished()), object, SLOT(newPagePublished()));
    QObject::connect(&worker, SIGNAL(newPagePublishFailed(QVariant)), object, SLOT(newPagePublishFailed(QVariant)));
    QObject::connect(&worker, SIGNAL(newPostPublished()), object, SLOT(newPostPublished()));
    QObject::connect(&worker, SIGNAL(newPostPublishFailed(QVariant)), object, SLOT(newPostPublishFailed(QVariant)));

    QObject::connect(&worker, SIGNAL(setPageModelStatus(QVariant,QVariant)), object, SIGNAL(setPageModelStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setPostModelStatus(QVariant,QVariant)), object, SIGNAL(setPostModelStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setCommentModelStatus(QVariant,QVariant)), object, SIGNAL(setCommentModelStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setMediaModelStatus(QVariant,QVariant)), object, SIGNAL(setMediaModelStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setAddPageStatus(QVariant,QVariant)), object, SIGNAL(setAddPageStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setAddPostStatus(QVariant,QVariant)), object, SIGNAL(setAddPostStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setAddCommentStatus(QVariant,QVariant)), object, SIGNAL(setAddCommentStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setAddMediaStatus(QVariant,QVariant)), object, SIGNAL(setAddMediaStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setAddCategoryStatus(QVariant,QVariant)), object, SIGNAL(setAddCategoryStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(setSearchMediaStatus(QVariant,QVariant)), object, SIGNAL(setSearchMediaStatus(QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(showPost(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
                     object, SLOT(showPost(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(addFileSuccessful(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
                     object, SLOT(addFileSuccessful(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));
    QObject::connect(&worker, SIGNAL(updateCurrentPostCategories(QVariant)),
                     object, SLOT(updateCurrentPostCategories(QVariant)));
    QObject::connect(object, SIGNAL(refreshThumbnailCache()), &worker, SLOT(refreshThumbnailCache()));
    QObject::connect(object, SIGNAL(saveSettings(QString,QString)),
                     &worker, SLOT(saveSettings(QString,QString)));

    QObject::connect(object, SIGNAL(getDirectory()), &worker, SLOT(getDirSelectionDialog()));
    QObject::connect(&worker, SIGNAL(setSelectedDir(QVariant)), object, SLOT(setSelectedDir(QVariant)));
    QObject::connect(object, SIGNAL(addNewDir(QString)),
                     &worker, SLOT(addNewDir(QString)));
    QObject::connect(object, SIGNAL(removeExistingDir(int)),
                     &worker, SLOT(removeExistingDir(int)));
    QObject::connect(&worker, SIGNAL(updateSettings(QVariant)),
                     object, SLOT(updateSettings(QVariant)));

    QObject::connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));

    worker.checkSettings();

#if defined(Q_OS_SYMBIAN)
    int platformId = 0;
#elif defined(Q_WS_MAEMO_5)
    int platformId = 2;
#elif defined(QT_SIMULATOR)
    int platformId = 3;
#elif defined(Q_WS_WIN)
    int platformId = 4;
#else
    // meego probably
    int platformId = 1;
#endif

    view.rootContext()->setContextProperty("platform",  platformId);
//    view.rootContext()->setContextProperty("isBlogFound",  worker.isBlogFound);

    view.rootContext()->setContextProperty("blogsModel", worker.blogsModel);
    view.rootContext()->setContextProperty("pagesModel", worker.pagesModel);
    view.rootContext()->setContextProperty("postsModel", worker.postsModel);
    view.rootContext()->setContextProperty("commentsModel", worker.commentsModel);
    view.rootContext()->setContextProperty("postCommentsModel", worker.postCommentsModel);
    view.rootContext()->setContextProperty("categoriesModel", worker.categoriesModel);
    view.rootContext()->setContextProperty("mediaModel", worker.mediaModel);
    view.rootContext()->setContextProperty("localMediaModel", worker.localMediaModel);
    view.rootContext()->setContextProperty("mediaDirModel", worker.mediaDirModel);

//    if(worker.isBlogFound)
    object->setProperty("isThemeInverted",  worker.uiTheme=="dark"?true:false);
    object->setProperty("isIconsMetro",  worker.uiIconType=="metro"?true:false);
    object->setProperty("isBlogFound", worker.isBlogFound);
    object->setProperty("blogCount", worker.blogsModel->rowCount());
    qDebug()<<"Current Bloog"<<worker.currentBlogName;
    object->setProperty("currentBlogName", worker.currentBlogName);
    object->setProperty("isCurrentBlogWPHosted", worker.isCurrentBlogWPHosted);
    object->setProperty("currentBlogPagesCount", worker.currentBlogPagesCount);
    object->setProperty("currentBlogPostsCount", worker.currentBlogPostsCount);
    object->setProperty("currentBlogCommentsCount", worker.currentBlogCommentsCount);
    object->setProperty("currentBlogMediaCount", worker.currentBlogMediaCount);
    object->setProperty("pageModelState", worker.pageModelState);
    object->setProperty("pageModelStatus", worker.pageModelStatus);
    object->setProperty("postModelState", worker.postModelState);
    object->setProperty("postModelStatus", worker.postModelStatus);
    object->setProperty("commentModelState", worker.commentModelState);
    object->setProperty("commentModelStatus", worker.commentModelStatus);
    object->setProperty("mediaModelState", worker.mediaModelState);
    object->setProperty("mediaModelStatus", worker.mediaModelStatus);
    object->setProperty("mediaModelState", worker.mediaModelState);
    object->setProperty("mediaModelStatus", worker.mediaModelStatus);
    object->setProperty("addPostState", worker.addPostState);
    object->setProperty("addPostStatus", worker.addPostStatus);
//    object->setProperty("addMediaState", worker.addMediaState);
//    object->setProperty("addMediaStatus", worker.addMediaStatus);
    attribute = Qt::WA_AutoOrientation;
    splash.setAttribute(attribute, true);
#if defined(Q_WS_WIN)
    view.show();
#else
    view.showFullScreen();
#endif
    splash.finish(&view);
    return app.exec();
}
//![0]
