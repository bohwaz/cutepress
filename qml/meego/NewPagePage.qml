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
    id: pagePage

    property bool isEditingPage: false
    property int mediaItemPosition: 0
    property string pageLocalId: ""
    property alias pageTitleText: pageTitle.text
    property alias pageContentText: pageText.text
    property alias pageDate: date.text
    property alias pageTime: time.text
    property alias pagePublishStatus: publishStatus.text
    property alias pagePwdText: pwdText.text

    function resetPage(){
        window.addPageState = UI.ProgressState.None
        window.addPageStatus = ""
        isEditingPage = false
        mediaItemPosition = 0
        pageLocalId = ""
        pageContentText = ""
        pageDate = ""
        pageTime = ""
        pagePublishStatus = ""
        pagePwdText = ""
    }

    function insertMedia(html)
    {
        console.log(qsTr("Insert Media"),html,window.tempPagePos, window.tempPageText, mediaItemPosition)
        pageText.text = window.tempPageText.substring(0,window.tempPagePos)+
                       html+
                       window.tempPageText.substring(window.tempPagePos)
        pageText.focus = true
        pageText.cursorPosition = window.tempPagePos+html.length
        pageText.forceActiveFocus()
    }

    tools: ToolBarLayout {
        enabled: window.addPageState != UI.ProgressState.Processing
        ToolIcon {
            id: button1
            iconSource: window.isThemeInverted?UI.BACKIMG:UI.BACKIMG
            onClicked: {
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
        visualParent: pagePage
        content: MenuLayout {
                MenuItem {
                    text: qsTr("Save as draft")
                    onClicked: {
                        if(isEditingPage)
                            window.editPage(pageLocalId,
                                            pageTitleText,
                                            pageContentText,
                                            pageDate+" "+pageTime,
                                            "draft")
                        else
                            window.addNewage(pageTitleText,
                                              pageContentText,
                                              pagePwdText,
                                              pageDate+" "+pageTime,
                                              "draft")
                    }
                }
                MenuItem {
                    text: qsTr("Save as local draft")
                    visible: pagePublishStatus==qsTr("Local Draft") || pagePublishStatus==""
                    onClicked: {
                        window.addNewPage(pageTitleText,
                                          pageContentText,
                                          pagePwdText,
                                          pageDate+" "+pageTime,
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
                        if(isEditingPage)
                            window.editPage(pageLocalId,
                                            pageTitleText,
                                            pageContentText,
                                            pagePwdText,
                                            pageDate+" "+pageTime,
                                            "publish")
                        else
                            window.addNewPage(pageTitleText,
                                              pageContentText,
                                              pagePwdText,
                                              pageDate+" "+pageTime,
                                              "publish")
                    }
                }
                MenuItem {
                    text: qsTr("Publish as private")
                    onClicked: {
                        if(isEditingPage)
                            window.editPage(pageLocalId,
                                            pageTitleText,
                                            pageContentText,
                                            pagePwdText,
                                            pageDate+" "+pageTime,
                                            "private")
                        else
                            window.addNewPage(pageTitleText,
                                              pageContentText,
                                              pagePwdText,
                                              pageDate+" "+pageTime,
                                              "private")
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
        visible: window.addPageState!=UI.ProgressState.None

        BigHeadingText {
            id: heading
            anchors.verticalCenter: parent.verticalCenter
            text: window.addPageStatus
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
            visible: window.addPageState == UI.ProgressState.Processing
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
        contentHeight: content.height
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: content
            visible: !pageStack.busy
            anchors.fill: parent
            anchors.margins: UI.MARGIN_XLARGE
            spacing: 8

            ListSectionDelegate {
                id: section
                sectionName: isEditingPage?"Edit Page":"Add Page"
                color: window.isThemeInverted?UI.LISTSECTION_TITLE_COLOR:UI.LISTSECTION_TITLE_COLOR
                margins: 0
            }
            Text {
                id: publishStatus
                width: parent.width
                font.pixelSize: window.appGeneralFontSize
                elide: Text.ElideRight
                color: {
                    if(text == qsTr("Published"))
                        return "green";
                    else if(text == qsTr("Private"))
                        return "lightgreen";
                    else if(text == qsTr("Scheduled in future"))
                        return "#5674b9";
                    else if(text == qsTr("Pending review"))
                        return "yellow";
                    else if(text == qsTr("Draft")||text == qsTr("Local Draft") || text == qsTr("Busy"))
                        return "orange";
                    else if(text == qsTr("Trashed")||text == qsTr("Orphaned"))
                        return "red";
                }
                visible: text!=""
            }

            HeadingText {
                text: qsTr("Title")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            TextField {
                id: pageTitle
                width: parent.width
//                font.pixelSize: window.appInputFontSize
                placeholderText: qsTr("Enter title here")
            }

            HeadingText {
                text: qsTr("Content")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            TextArea {
                id: pageText
                textFormat: Text.PlainText
//                font.pixelSize: window.appInputFontSize
                placeholderText: qsTr("Enter content here")
                width: parent.width
                height: Math.max(450, implicitHeight)
                onActiveFocusChanged: {
                    if(activeFocus){
                        toolTimer.stop()
                        contentTool.visible = true
                    }
                    else
                        toolTimer.start()
                }
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
                    onClicked: pagePage.launchDialogToToday()
                }
                Button {
                    id: time
                    text: qsTr("Time")
                    font.pixelSize: window.appInputFontSize - 1
                    width: (parent.width - 2*parent.spacing - dateLabel.width)*1/2
                    onClicked: pagePage.launchDialogToNow()
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

    LinkDialog {
        id: linkDialog
        titleText: qsTr("Enter the destination URL")
        linkTitleText: ""
        rejectButtonText: qsTr("Cancel")
        acceptButtonText: qsTr("Add Link")
        onRejected: pageText.focus = true
        onAccepted: {
            var pre = "<a href=\'"+linkUrlText+"\'>"
            pageText.text = pageText.text.substring(0,Math.min(selectionStart,selectionEnd))+
                    pre+linkTitleText+"</a>"+
                    pageText.text.substring(Math.max(selectionStart,selectionEnd),pageText.text.length)
            if(pageText.textFormat == Text.PlainText)
                pageText.cursorPosition = cursorPos + pre.length
            pageText.forceActiveFocus()
        }
    }
    Rectangle {
        id: contentTool
        color: window.isThemeInverted?"black":"#CCCCCC"
        width: parent.width
        height: toolGrid.childrenRect.height + 10
        radius: 10
        visible: false

        Grid {
            id: toolGrid
            columns: 7
            width: 0.96*parent.width
            anchors.centerIn: parent
            spacing: 5

            Button {
                id: boldButton
                width: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: width
                text: "b"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    var cPos = pageText.cursorPosition
                    /*var selStart = pageText.selectionStart
                    var selEnd = pageText.selectionEnd
                    pageText.text = pageText.text.substring(0,Math.min(pageText.selectionStart,pageText.selectionEnd))+
                            "<strong>"+pageText.text.substring(Math.min(pageText.selectionStart,pageText.selectionEnd),Math.max(pageText.selectionStart,pageText.selectionEnd))+"</strong>"+
                            pageText.text.substring(Math.max(pageText.selectionStart,pageText.selectionEnd),pageText.text.length)
                    if(pageText.textFormat == Text.PlainText)
                        pageText.cursorPosition = cPos + 8*/
                    if(pageText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "</strong>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 9
                        }
                        else {
                            checked = true
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "<strong>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 8
                        }
                    }
                    pageText.forceActiveFocus()
                }
            }
            Button {
                width: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: width
                text: "i"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    var cPos = pageText.cursorPosition
                    /*var selStart = pageText.selectionStart
                    var selEnd = pageText.selectionEnd
                    pageText.text = pageText.text.substring(0,Math.min(pageText.selectionStart,pageText.selectionEnd))+
                            "<em>"+pageText.text.substring(Math.min(pageText.selectionStart,pageText.selectionEnd),Math.max(pageText.selectionStart,pageText.selectionEnd))+"</em>"+
                            pageText.text.substring(Math.max(pageText.selectionStart,pageText.selectionEnd),pageText.text.length)
                    if(pageText.textFormat == Text.PlainText)
                        pageText.cursorPosition = cPos + 4*/
                    if(pageText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "</em>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 5
                        }
                        else {
                            checked = true
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "<em>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 4
                        }
                    }
                    pageText.forceActiveFocus()
                }
            }
            Button {
                width: 1.75*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "b-quote"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    var cPos = pageText.cursorPosition
                    /*var selStart = pageText.selectionStart
                    var selEnd = pageText.selectionEnd
                    pageText.text = pageText.text.substring(0,Math.min(pageText.selectionStart,pageText.selectionEnd))+
                            "\n<blockquote>"+pageText.text.substring(Math.min(pageText.selectionStart,pageText.selectionEnd),Math.max(pageText.selectionStart,pageText.selectionEnd))+"</blockquote>"+
                            pageText.text.substring(Math.max(pageText.selectionStart,pageText.selectionEnd),pageText.text.length)
                    if(pageText.textFormat == Text.PlainText)
                        pageText.cursorPosition = cPos + 13*/
                    if(pageText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "</blockquote>\n"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 14
                        }
                        else {
                            checked = true
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "\n<blockquote>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 13
                        }
                    }
                    pageText.forceActiveFocus()
                }
            }
            Button {
                width: 1.25*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "code"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    var cPos = pageText.cursorPosition
                    /*var selStart = pageText.selectionStart
                    var selEnd = pageText.selectionEnd*/
                    if(pageText.textFormat == Text.PlainText) {
                        if(checked) {
                            checked = false
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "</code>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 7
                        }
                        else {
                            checked = true
                            pageText.text = pageText.text.substring(0, pageText.cursorPosition)+
                                            "<code>"+
                                            pageText.text.substring(pageText.cursorPosition,pageText.text.length)
                            pageText.cursorPosition = cPos + 6
                        }
                    }
                    pageText.forceActiveFocus()
                    /*pageText.text = pageText.text.substring(0,Math.min(pageText.selectionStart,pageText.selectionEnd))+
                                    "<code>"+pageText.text.substring(Math.min(pageText.selectionStart,pageText.selectionEnd),Math.max(pageText.selectionStart,pageText.selectionEnd))+"</code>"+
                                    pageText.text.substring(Math.max(pageText.selectionStart,pageText.selectionEnd),pageText.text.length)*/
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "link"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    linkDialog.linkTitleText = pageText.selectedText
                    linkDialog.linkUrlText = "http://"
                    linkDialog.selectionStart = pageText.selectionStart
                    linkDialog.selectionEnd = pageText.selectionEnd
                    linkDialog.cursorPos = pageText.cursorPosition
                    linkDialog.open()
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "img"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    window.tempPageText = pageText.text
                    window.tempPagePos = pageText.cursorPosition
                    mediaItemPosition = pageText.cursorPosition
                    console.log("Hi hello", window.tempPagePos, window.tempPageText, window.addMediaState, window.addMediaStatus)
                    window.searchMedia("image")
                    window.ampMediaType = "image"
                    window.ampCaller = "page"
                    window.replaceFile("AddMediaPage.qml")
                    pageText.forceActiveFocus()
                }
            }
            Button {
                width: 1.1*(UI.TOOLBUTTON_SMALL_WIDTH+10)
                height: (UI.TOOLBUTTON_SMALL_WIDTH+10)
                text: "vid"
                font.pixelSize: window.appGeneralFontSize - 3
                onClicked: {
                    window.tempPageText = pageText.text
                    window.tempPagePos = pageText.cursorPosition
                    window.searchMedia("video")
                    window.ampMediaType = "video"
                    window.ampCaller = "page"
                    window.replaceFile("AddMediaPage.qml")
                    pageText.forceActiveFocus()
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
