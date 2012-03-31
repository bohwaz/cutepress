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
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../UIConstants.js" as UI

CPPage {
    id: postPage

    property bool isEditingPost: false
    property int mediaItemPosition: 0
    property string postLocalId: ""
    property alias postTitleText: postTitle.text
    property alias postPageText: postText.text
    property alias postTagsText: tagsText.text
    property alias postCatText: catText.text
    property alias postPwdText: pwdText.text
    property alias postDate: date.text
    property alias postTime: time.text
    property alias postPublishStatus: publishStatus.text

    function resetPage(){
        window.addPostState = UI.ProgressState.None
        window.addPostStatus = ""
        isEditingPost = false
        mediaItemPosition = 0
        postLocalId = ""
        postTitleText = ""
        postPageText = ""
        postTagsText = ""
        postCatText = ""
        postPwdText = ""
        postDate = ""
        postTime = ""
        postPublishStatus = ""
    }

    function insertMedia(html)
    {
        console.log(qsTr("Insert Media"),html,window.tempPostPos, window.tempPostText, mediaItemPosition)
        postText.text = window.tempPostText.substring(0,window.tempPostPos)+
                       html+
                       window.tempPostText.substring(window.tempPostPos)
        postText.focus = true
        postText.cursorPosition = window.tempPostPos+html.length
    }

    tools: ToolBarLayout {
        enabled: window.addPostState != UI.ProgressState.Processing
        ToolIcon {
            id: button1
            iconSource: window.isThemeInverted?UI.BACKIMG:UI.BACKIMG
            onClicked: {
                window.unmarkAllCategories()
                pageStack.pop()
            }
        }
//        ToolButton {
//            id: button2
//            flat: true
//            iconSource:"qrc:/qml/images/reset.png"
//        }
        ToolIcon {
            id: button3
            iconSource: window.isThemeInverted?UI.SAVEIMG:UI.SAVEIMG
            onClicked: saveMenu.open()
        }
        ToolIcon {
            id: button4
            iconSource: window.isThemeInverted?UI.PUBLISHIMG:UI.PUBLISHIMG
            onClicked: pubMenu.open()
        }
    }
    Menu {
        id: saveMenu
        visualParent: postPage
        content: MenuLayout {
                MenuItem {
                    text: qsTr("Save as draft")
                    onClicked: {
                        if(isEditingPost)
                            window.editPost(postLocalId,
                                            postTitleText,
                                            postPageText,
                                            postTagsText,
                                            postDate+" "+postTime,
                                            postPwdText,
                                            "draft")
                        else
                            window.addNewPost(postTitleText,
                                            postPageText,
                                            postTagsText,
                                            postDate+" "+postTime,
                                            postPwdText,
                                            "draft")
                    }
                }
                MenuItem {
                    text: qsTr("Save as local draft")
                    visible: postPublishStatus==qsTr("Local Draft") || postPublishStatus==""
                    onClicked: {
                        window.addNewPost(postTitleText,
                                          postPageText,
                                          postTagsText,
                                          postDate+" "+postTime,
                                          postPwdText,
                                          "local draft")
                        pageStack.pop()
                    }
                }
            }
    }

    Menu {
        id: pubMenu
        visualParent: toolBar
        content: MenuLayout {
                MenuItem {
                    text: qsTr("Publish as public")
                    onClicked: {
                        if(isEditingPost)
                            window.editPost(postLocalId,
                                            postTitleText,
                                            postPageText,
                                            postTagsText,
                                            postDate+" "+postTime,
                                            postPwdText,
                                            "publish")
                        else
                            window.addNewPost(postTitleText,
                                              postPageText,
                                              postTagsText,
                                              postDate+" "+postTime,
                                              postPwdText,
                                              "publish")
                    }
                }
                MenuItem {
                    text: qsTr("Publish as private")
                    onClicked: {
                        if(isEditingPost)
                            window.editPost(postLocalId,
                                            postTitleText,
                                            postPageText,
                                            postTagsText,
                                            postDate+" "+postTime,
                                            postPwdText,
                                            "private")
                        else
                            window.addNewPost(postTitleText,
                                              postPageText,
                                              postTagsText,
                                              postDate+" "+postTime,
                                              postPwdText,
                                              "private")
                    }
                }
            }
    }
    Component {
        id: categoryDelegate
        Item {
            id: wrapper
            visible: isMarked
            width: wrapper.ListView.view.width
            height: Math.max(titleText.height, 48)

            Rectangle { id: bgRect; anchors.fill: parent; color: window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}
            Text {
                id: titleText
                text: catName
                color: isMarked?UI.LISTDELEGATE_TEXT_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TITLE_COLOR:UI.LISTDELEGATE_TITLE_COLOR
                textFormat: Text.RichText
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                font.pixelSize: window.appGeneralFontSize
                font.bold: isMarked
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    leftMargin: 5
                    rightMargin: 5
                }
            }
        }
    }

    Row {
        id: header
        spacing: 5
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: UI.MARGIN_XLARGE
        }
        visible: window.addPostState!=UI.ProgressState.None

        BigHeadingText {
            id: heading
            anchors.verticalCenter: parent.verticalCenter
            text: window.addPostStatus
            width: parent.width-parent.spacing-status.width
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            color: window.isThemeInverted?UI.BIGHEADINGTEXT_COLOR:UI.BIGHEADINGTEXT_COLOR
        }

        BusyIndicator {
            id: status
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            platformStyle: BusyIndicatorStyle { size: "small" }
            visible: window.addPostState == UI.ProgressState.Processing
            running: true
        }
    }

    Flickable {
        id: pageFlickable
        clip: true
        anchors{
            top: header.visible?header.bottom:parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: UI.MARGIN_XLARGE
        }
        contentHeight: content.childrenRect.height+2*UI.MARGIN_XLARGE
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: content
            visible: !pageStack.busy
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            spacing: 8

            ListSectionDelegate {
                id: section
                sectionName: isEditingPost?qsTr("Edit Post"):qsTr("Add Post")
                color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
                margins: 0
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: publishStatus.height+4
                visible: publishStatus.text!=""
                color: {
                    if(publishStatus.text == qsTr("Published"))
                        return "green";
                    else if(publishStatus.text == qsTr("Private"))
                        return "lightgreen";
                    else if(publishStatus.text == qsTr("Scheduled in future"))
                        return "#5674b9";
                    else if(publishStatus.text == qsTr("Pending review"))
                        return "yellow";
                    else if(publishStatus.text == qsTr("Draft")||publishStatus.text == qsTr("Local Draft") || publishStatus.text == qsTr("Busy"))
                        return "orange";
                    else if(publishStatus.text == qsTr("Trashed")||publishStatus.text == qsTr("Orphaned"))
                        return "red";
                }

                Text {
                    id: publishStatus
                    text: window.sppPostPublishStatus;
                    font.pixelSize: window.appGeneralFontSize-1
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    elide: Text.ElideRight
                }
            }

            HeadingText {
                text: qsTr("Title")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            TextField {
                id: postTitle
                width: parent.width
//                font.pixelSize: window.appInputFontSize
                placeholderText: qsTr("Enter title here")
            }

            HeadingText {
                text: qsTr("Content")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }
            TextArea {
                id: postText
                anchors.left: parent.left
                textFormat: Text.PlainText
//                font.pixelSize: window.appInputFontSize
                width: parent.width
                height: Math.max(225, implicitHeight)
                onActiveFocusChanged: {
                    if(activeFocus){
                        toolTimer.stop()
                        contentTool.visible = true
                    }
                    else
                        toolTimer.start()
                }
            }

            HeadingText {
                text: qsTr("Tags")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            TextArea {
                id: tagsText
                width: parent.width
//                font.pixelSize: window.appInputFontSize
                placeholderText: qsTr("Enter tags here")
                height: 72
            }

            Row {
                width: parent.width
                spacing: 5



                HeadingText {
                    text: qsTr("Categories")
                    width: 120
                    anchors.verticalCenter: parent.verticalCenter
                    color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
                }
                ToolIcon {
                    id: addCatButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: window.isThemeInverted?UI.ADDIMG:UI.ADDIMG
                    onClicked: window.openFile("AddCategoriesPage.qml")
                }
//                Button {
//                    text: qsTr("Add Categories")
//                    font.pixelSize: window.appInputFontSize
//                    width: (parent.width - parent.spacing)/2
//                    onClicked: window.openFile("AddCategoriesPage.qml")
//                }
//                Button {
//                    text: qsTr("Add Location")
//                    font.pixelSize: window.appInputFontSize
//                    width: (parent.width - parent.spacing)/2
//                }
            }

            Text {
                id: catText
                width: parent.width
                wrapMode: Text.Wrap
                color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
                font.pixelSize: window.appGeneralFontSize
            }

            Row {
                width: parent.width
                spacing: 5

                HeadingText {
                    id: dateLabel
                    width: 150
                    text: qsTr("Published on")
                    anchors.verticalCenter: parent.verticalCenter
                    color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
                }
                Button {
                    id: date
                    text: qsTr("Date")
                    font.pixelSize: window.appInputFontSize - 1
                    width: (parent.width - 2*parent.spacing - dateLabel.width)*1/2
                    onClicked: postPage.launchDialogToToday()
                }
                Button {
                    id: time
                    text: qsTr("Time")
                    font.pixelSize: window.appInputFontSize - 1
                    width: (parent.width - 2*parent.spacing - dateLabel.width)*1/2
                    onClicked: postPage.launchDialogToNow()
                }
            }

            Row {
                width: parent.width
                spacing: 10
                HeadingText {
                    id: pwdLabel
                    width: 120
                    text: qsTr("Password")
                    anchors.verticalCenter: parent.verticalCenter
                    color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
                }
                TextField {
                    id: pwdText
                    width: parent.width - pwdLabel.width - parent.spacing
//                    font.pixelSize: window.appInputFontSize
                    placeholderText: qsTr("Enter password here")
                }
            }
        }
    }
    Rectangle {
        id: contentTool
        color: window.isThemeInverted?"black":"#CCCCCC"
        width: parent.width
        height: toolGrid.childrenRect.height + 10
        visible: postText.activeFocus
        radius: 10

        Grid {
            id: toolGrid
            columns: 7
            width: 0.96*parent.width
            anchors.centerIn: parent
            spacing: 5

            Button {
                width: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: width
                text: "b"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    console.log("Hell Yeah!")
                    var cPos = postText.cursorPosition
                    /*var selStart = postText.selectionStart
                    var selEnd = postText.selectionEnd
                    postText.text = postText.text.substring(0,Math.min(postText.selectionStart,postText.selectionEnd))+
                            "<strong>"+postText.text.substring(Math.min(postText.selectionStart,postText.selectionEnd),Math.max(postText.selectionStart,postText.selectionEnd))+"</strong>"+
                            postText.text.substring(Math.max(postText.selectionStart,postText.selectionEnd),postText.text.length)
                    if(postText.textFormat == Text.PlainText)
                        postText.cursorPosition = cPos + 8*/
                    if(postText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "</strong>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 9
                        }
                        else {
                            checked = true
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "<strong>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 8
                        }
                    }
                    postText.forceActiveFocus()
                }
            }
            Button {
                width: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: width
                text: "i"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    var cPos = postText.cursorPosition
                    /*var selStart = postText.selectionStart
                    var selEnd = postText.selectionEnd
                    postText.text = postText.text.substring(0,Math.min(postText.selectionStart,postText.selectionEnd))+
                            "<em>"+postText.text.substring(Math.min(postText.selectionStart,postText.selectionEnd),Math.max(postText.selectionStart,postText.selectionEnd))+"</em>"+
                            postText.text.substring(Math.max(postText.selectionStart,postText.selectionEnd),postText.text.length)
                    if(postText.textFormat == Text.PlainText)
                        postText.cursorPosition = cPos + 4*/
                    if(postText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "</em>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 5
                        }
                        else {
                            checked = true
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "<em>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 4
                        }
                    }
                    postText.forceActiveFocus()
                }
            }
            Button {
                width: 1.75*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "b-quote"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    var cPos = postText.cursorPosition
                    /*var selStart = postText.selectionStart
                    var selEnd = postText.selectionEnd
                    postText.text = postText.text.substring(0,Math.min(postText.selectionStart,postText.selectionEnd))+
                            "\n<blockquote>"+postText.text.substring(Math.min(postText.selectionStart,postText.selectionEnd),Math.max(postText.selectionStart,postText.selectionEnd))+"</blockquote>"+
                            postText.text.substring(Math.max(postText.selectionStart,postText.selectionEnd),postText.text.length)
                    if(postText.textFormat == Text.PlainText)
                        postText.cursorPosition = cPos + 13*/
                    if(postText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "</blockquote>\n"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 14
                        }
                        else {
                            checked = true
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "\n<blockquote>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 13
                        }
                    }
                    postText.forceActiveFocus()
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "code"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    var cPos = postText.cursorPosition
                    /*var selStart = postText.selectionStart
                    var selEnd = postText.selectionEnd*/
                    if(postText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "</code>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 7
                        }
                        else {
                            checked = true
                            postText.text = postText.text.substring(0, postText.cursorPosition)+
                                            "<code>"+
                                            postText.text.substring(postText.cursorPosition,postText.text.length)
                            postText.cursorPosition = cPos + 6
                        }
                    }
                    postText.forceActiveFocus()
                    /*postText.text = postText.text.substring(0,Math.min(postText.selectionStart,postText.selectionEnd))+
                                    "<code>"+postText.text.substring(Math.min(postText.selectionStart,postText.selectionEnd),Math.max(postText.selectionStart,postText.selectionEnd))+"</code>"+
                                    postText.text.substring(Math.max(postText.selectionStart,postText.selectionEnd),postText.text.length)*/
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "link"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    linkDialog.linkTitleText = postText.selectedText
                    linkDialog.linkUrlText = "http://"
                    linkDialog.selectionStart = postText.selectionStart
                    linkDialog.selectionEnd = postText.selectionEnd
                    linkDialog.cursorPos = postText.cursorPosition
                    linkDialog.open()
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "img"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    window.tempPostText = postText.text
                    window.tempPostPos = postText.cursorPosition
                    mediaItemPosition = postText.cursorPosition
                    console.log("Hi hello", window.tempPostPos, window.tempPostText, window.addMediaState, window.addMediaStatus)
                    window.searchMedia("image")
                    window.ampMediaType = "image"
                    window.ampCaller = "post"
                    window.replaceFile("AddMediaPage.qml")
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "vid"
                font.pixelSize: window.appGeneralFontSize - 4
                onClicked: {
                    window.tempPostText = postText.text
                    window.tempPostPos = postText.cursorPosition
                    window.searchMedia("video")
                    window.ampMediaType = "video"
                    window.ampCaller = "post"
                    window.replaceFile("AddMediaPage.qml")
                }
            }
        }

        Timer {
            id: toolTimer
            interval: 1280
            repeat: false
            onTriggered: parent.visible = false
        }
    }


    LinkDialog {
        id: linkDialog
        titleText: qsTr("Enter the destination URL")
        linkUrlText: "http://"
        linkTitleText: ""
        rejectButtonText: qsTr("Cancel")
        acceptButtonText: qsTr("Add Link")
        onRejected: postText.focus = true
        onAccepted: {
            var pre = "<a href=\'"+linkUrlText+"\'>"
            postText.text = postText.text.substring(0,Math.min(selectionStart,selectionEnd))+
                    pre+linkTitleText+"</a>"+
                    postText.text.substring(Math.max(selectionStart,selectionEnd),postText.text.length)
            if(postText.textFormat == Text.PlainText)
                postText.cursorPosition = cursorPos + pre.length
            postText.forceActiveFocus()
        }
    }

    DatePickerDialog {
         id: tDialog
         titleText: qsTr("Publish Date")
         acceptButtonText: qsTr("Ok")
         rejectButtonText: qsTr("Cancel")
         onAccepted: date.text = tDialog.year + "/" + tDialog.month + "/" + tDialog.day
     }

    TimePickerDialog{
        id: timeDialog
        titleText: qsTr("Publish Time")
        acceptButtonText: qsTr("Ok")
        rejectButtonText: qsTr("Cancel")
        fields: DateTime.All
        onAccepted: time.text = timeDialog.hour + ":" + timeDialog.minute + ":" + timeDialog.second
    }

    function launchDialogToToday() {
        var d = new Date();
        tDialog.year = d.getFullYear();
        tDialog.month = d.getMonth()+1;
        tDialog.day = d.getDate();
        tDialog.open();
    }

    function launchDialogToNow() {
        var d = new Date();
        //timeDialog.hourMode =
        timeDialog.hour = d.getHours();
        timeDialog.minute = d.getMinutes();
        timeDialog.second = d.getSeconds();
        timeDialog.open();
    }
}
//![0]
