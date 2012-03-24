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

import QtQuick 1.0
import com.nokia.symbian 1.0
import com.nokia.extras 1.0
import "../UIConstants.js" as UI

//![0]
Window {
    id: window

    Rectangle {
        id: background

        anchors.fill: parent
        color: isThemeInverted?UI.PAGE_BG_COLOR:UI.PAGE_BG_COLOR1
        Image {
            visible: isThemeInverted
            source: "qrc:/qml/images/pagebg.jpg"
            fillMode: Image.Stretch
            anchors.fill: parent
        }
    }
    property bool isThemeInverted: true
    property bool isIconsMetro: true
    onIsThemeInvertedChanged: {
        console.log("UI Theme", isThemeInverted)
//        if(isThemeInverted!=theme.inverted) {
//            console.log("Inverting", isThemeInverted)
//            theme.inverted = isThemeInverted
//        }
        if(isThemeInverted==UI.IS_THEME_LIGHT)
            UI.switchTheme()
    }

    property int appGeneralFontSize: platformStyle.fontSizeSmall + 1
    property int appSectionFontSize: platformStyle.fontSizeSmall - 2
    property int appInputFontSize: platformStyle.fontSizeSmall+1

    property bool openActiveBlog: false
    property bool isBlogFound: false
    property int blogCount: 0
    property string currentBlogName: ""
    property bool isCurrentBlogWPHosted: false
    property int currentBlogPagesCount: 0
    property int currentBlogPostsCount: 0
    property int currentBlogCommentsCount: 0
    property int currentBlogMediaCount: 0
    property int pageModelState: UI.ProgressState.None
    property string pageModelStatus: ""
    property int postModelState: UI.ProgressState.None
    property string postModelStatus: ""
    property int commentModelState: UI.ProgressState.None
    property string commentModelStatus: ""
    property int mediaModelState: UI.ProgressState.None
    property string mediaModelStatus: ""
    property int addPageState: UI.ProgressState.None
    property string addPageStatus: ""
    property int addPostState: UI.ProgressState.None
    property string addPostStatus: ""
    property int addCommentState: UI.ProgressState.None
    property string addCommentStatus: ""
    property int addMediaState: UI.ProgressState.None
    property string addMediaStatus: ""
    property int addCategoryState: UI.ProgressState.None
    property string addCategoryStatus: ""
    property int searchMediaState: UI.ProgressState.None
    property string searchMediaStatus: ""
    property string newSelectedDir: ""

    property string addNewBlogMsg: ""
    property int addNewBlogStatus: UI.ProgressState.None

    property string tempPostText: ""
    property int tempPostPos: 0
    property string tempPageText: ""
    property int tempPagePos: 0
    property bool addingMediaToPost: false

    onAddNewBlogStatusChanged: console.log("Status Chaanged",addNewBlogStatus,addNewBlogMsg)

    onIsBlogFoundChanged: {
        console.log("123456")
        if(isBlogFound)
            pageStack.push(blogOptionsPage)
    }


    signal addNewBlog(string url,string usr,string pwd,bool resizecheck)
    signal editBlog(string id, string url,string usr,string pwd,bool resizecheck)
    signal addNewPage(string title, string content, string password, string date, string status)
    signal editPage(string id, string title, string content, string password, string date, string status)
    signal addNewPost(string title, string content, string tags, string date, string pass, string status)
    signal editPost(string id, string title, string content, string tags, string date, string pass, string status)
    signal addNewCategory(string title)
    signal refreshCurrentBlog()
    signal refreshCurrentBlogPages()
    signal refreshCurrentBlogPosts()
    signal refreshCurrentBlogComments()
    signal refreshCurrentBlogMedia()
    signal uiReady()
    signal openBlog(string id)
    signal openPost(string id)
    signal replyToComment(string id, string pId)
    signal deleteBlog(string id)
    signal deletePage(string id)
    signal deletePost(string id)
    signal approveComment(string id)
    signal unApproveComment(string id)
    signal deleteComment(string id)
    signal spamComment(string id)
    signal addComment(string postId, string comment)
    signal searchMedia(string type)
    signal writeMediaItemsToModel(int count)
    signal addFile(string file, string type)
    signal markCurrentPostCategories(string id)
    signal markCategory(string cat)
    signal setCategoriesToPost(string id)
    signal unmarkAllCategories()
    signal refreshThumbnailCache()
    signal removeExistingDir(int value)
    signal addNewDir(string value)
    signal getDirectory()
    signal saveSettings(string theme, string icons)

    //SingleMediaPage vars

    property string smpThumbSource: ""
    property string smpNameText: ""
    property string smpDescText: ""
    property string smpCaptionText: ""
    property string smpTypeText: ""
    property string smpDateText: ""
    property string smpUrlText: ""

    //InsertMediaPage vars

    property string impThumbSource: ""
    property string impNameText: ""
    property string impDescText: ""
    property string impCaptionText: ""
    property string impTypeText: ""
    property string impDateText: ""
    property string impUrlText: ""
    property string impCaller: ""

    //AddMediaPage vars

    property string ampMediaType: ""
    property string ampCaller: ""

    //NewMediaPage vars

    property string nmpMediaType: ""

    //SinglePostPage vars

    property string sppPostId: ""
    property string sppPostTitle: ""
    property string sppPostHtml: ""
    property string sppPostCategories: ""
    property string sppPostDate: ""
    property string sppPostLink: ""
    property string sppPostCommentsCount: ""
    property string sppPostAuthor: ""
    property string sppPostPublishStatus: ""
    property string sppPostPassword: ""

    //NewPagePage vars

    property bool npageIsEditingPage: false
    property int npageMediaItemPosition: 0
    property string npageLocalId: ""
    property string npageTitleText: ""
    property string npageContentText: ""
    property string npageDate: ""
    property string npageTime: ""
    property string npagePublishStatus: ""

    //NewPagePage vars

    property string nbpBlogId: ""
    property bool nbpIsEditingBlog: false
    property string nbpUrlText: "http://"
    property string nbpUsernameText: ""
    property string nbpPasswordText: ""
    property bool nbpResizeCheck: false

    function smpClear(){
        smpThumbSource = ""
        smpNameText = ""
        smpDescText = ""
        smpCaptionText = ""
        smpTypeText = ""
        smpDateText = ""
        smpUrlText = ""
    }

    function impClear(){
        impThumbSource = ""
        impNameText = ""
        impDescText = ""
        impCaptionText = ""
        impTypeText = ""
        impDateText = ""
        impUrlText = ""
        impCaller = ""
    }

    function sppClear(){
        sppPostId = ""
        sppPostTitle = ""
        sppPostHtml = ""
        sppPostCategories = ""
        sppPostDate = ""
        sppPostLink = ""
        sppPostCommentsCount = ""
        sppPostAuthor = ""
        sppPostPublishStatus = ""
        sppPostPassword = ""
    }

    function npageClear(){
        npageIsEditingPage = false
        npageMediaItemPosition = 0
        npageLocalId = ""
        npageTitleText = ""
        npageContentText = ""
        npageDate = ""
        npageTime = ""
        npagePublishStatus = ""
    }

    function nbpClear(){
        window.addNewBlogMsg = ""
        window.addNewBlogStatus = UI.ProgressState.None
        nbpBlogId = ""
        nbpIsEditingBlog = false
        nbpUrlText = "http://"
        nbpUsernameText = ""
        nbpPasswordText = ""
        nbpResizeCheck = false
    }

    function openFile(file) {
        var component = Qt.createComponent(file)
        if (component.status == Component.Ready) {
            pageStack.push(component);
        }
        else
            console.log("Error loading component:", component.errorString());
    }

    function replaceFile(file) {
        var component = Qt.createComponent(file)
        if (component.status == Component.Ready) {
            pageStack.replace(component);
        }
        else
            console.log("Error loading component:", component.errorString());
    }

    /*Doesn't works!*/
    function blogFound(value)
    {
        console.log("Hi Abhishek! A blog is found",value)
        if(value)
            pageStack.push(blogOptionsPage);
    }

    function updateCurrentBlogDetails(name,isWP){
        if(window.nbpIsEditingBlog){
            addNewBlogStatus = UI.ProgressState.Success
            blogsViewPage.openCurrentBlog()
            addNewBlogStatus = UI.ProgressState.None
            nbpClear()
            console.log("Bloog editing finished")
        }
        console.log("BloogName")
        currentBlogName = name
        isCurrentBlogWPHosted = isWP
    }

    function updateCurrentBlogPagesCount(value){
        console.log("yes")
        currentBlogPagesCount = value
    }

    function updateCurrentBlogPostsCount(value){
        console.log("yes1")
        currentBlogPostsCount = value
    }

    function updateCurrentBlogCommentsCount(value){
        currentBlogCommentsCount = value
    }

    function updateCurrentBlogMediaCount(value){
        currentBlogMediaCount = value
    }

    function getUserBlogUnsuccessful(msg){
        addNewBlogMsg = msg
        addNewBlogStatus = UI.ProgressState.Error
    }

    function getUserBlogSuccessful(){
        console.log("New Bloog Added in UI")
        console.log(pageStack.busy,toolBar.visible,toolBar.enabled)
        addNewBlogStatus = UI.ProgressState.Success
        blogsViewPage.openCurrentBlog()
        //openActiveBlog = true
        //openFile("BlogOptionsPage.qml")
        addNewBlogStatus = UI.ProgressState.None
        //pageStack.pop(newBlogPage)
        console.log("New Bloog Added in UI Completed")
    }

    function setPageModelStatus(state, status){
        pageModelState = state
        pageModelStatus = status
    }

    function setPostModelStatus(state, status){
        postModelState = state
        postModelStatus = status
    }

    function setCommentModelStatus(state, status){
        commentModelState = state
        commentModelStatus = status
    }

    function setMediaModelStatus(state, status){
        mediaModelState = state
        mediaModelStatus = status
    }

    function setAddPageStatus(state, status){
        addPageState = state
        addPageStatus = status
    }

    function setAddPostStatus(state, status){
        addPostState = state
        addPostStatus = status
    }

    function setAddCommentStatus(state, status){
        addCommentState = state
        addCommentStatus = status
    }

    function setAddMediaStatus(state, status){
        addMediaState = state
        addMediaStatus = status
    }

    function setAddCategoryStatus(state, status){
        addCategoryState = state
        addCategoryStatus = status
    }

    function setSearchMediaStatus(state, status){
        searchMediaState = state
        searchMediaStatus = status
    }


    function showPost(id,title,content,date,categories,link,commentsCount,author,password,publishStatus){
        console.log("Show Poost")
        window.sppClear()
        sppPostId = id
        sppPostTitle = title
        sppPostHtml = content
        sppPostCategories = categories
        sppPostDate = date
        sppPostLink = link
        sppPostCommentsCount = commentsCount
        sppPostAuthor = author
        sppPostPublishStatus = publishStatus
        sppPostPassword = password
        window.openFile("SinglePostPage.qml")
    }

    function addFileSuccessful(thumb,filename,type,date,title,url,html)
    {
        console.log("Aaj Tak")
        if(addingMediaToPost){
            window.impClear()
            window.impThumbSource = thumb
            window.impNameText = filename
            window.impDescText = ""
            window.impCaptionText = ""
            window.impTypeText = type
            window.impDateText = date
            window.impUrlText = url
            window.impCaller = window.ampCaller
            window.replaceFile("InsertMediaPage.qml")
            addingMediaToPost = false
        }
        else{
            window.smpClear()
            window.smpThumbSource = thumb
            window.smpNameText = filename
            window.smpDescText = ""
            window.smpCaptionText = ""
            window.smpTypeText = type
            window.smpDateText = date
            window.smpUrlText = url
            window.replaceFile("SingleMediaPage.qml")
        }
    }

    function updateCurrentPostCategories(cat)
    {
        console.log("Setting categories in UI")
        newPostPage.postCatText = cat
    }

    function newPostPublished()
    {
        pageStack.pop()
    }

    function newPostPublishFailed(id)
    {
        newPostPage.postLocalId = id
        newPostPage.postPublishStatus = qsTr("Local Draft")
        newPostPage.isEditingPost = true
    }

    function newPagePublished()
    {
        pageStack.pop()
    }

    function newPagePublishFailed(id)
    {
        newPagePage.pageLocalId = id
        newPagePage.pagePublishStatus = qsTr("Local Draft")
        newPagePage.isEditingPage = true
    }

    function setSelectedDir(dir) {
        newSelectedDir = dir
    }

    function updateSettings (theme){
        if(theme=="light"){
            isThemeInverted = !isThemeInverted
        }
    }

    PageStack {
        id: pageStack
        anchors.fill: parent
        toolBar: toolBar
//        onDepthChanged: console.log("hall")
    }


    StatusBar {
        id: statusBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
    }

    ToolBar {
        id: toolBar
        enabled: !pageStack.busy
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
    }

    Menu {
        id: myMenu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    openFile("SettingsPage.qml")
                }
            }
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    openFile("AboutPage.qml")
                }
            }
            MenuItem {
                text: qsTr("Help")
                onClicked: {
                    openFile("HelpPage.qml")
                }
            }
            MenuItem {
                visible: pageStack.currentPage!=blogsViewPage
                text: qsTr("Exit")
                onClicked: Qt.quit()
            }
        }
    }

    BlogsViewPage { id: blogsViewPage }
    BlogOptionsPage { id: blogOptionsPage }
    NewPostPage { id: newPostPage }
    NewPagePage { id: newPagePage }

    Component.onCompleted: {
        pageStack.push(blogsViewPage);
        console.log("Hi Abhishek! Component loading complete.")
    }
}
//![0]
