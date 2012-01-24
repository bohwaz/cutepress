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
import QtQuick 1.0
import com.nokia.symbian 1.0
import "../UIConstants.js" as UI

CPPage {

    property bool marking: false

    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
    tools: ToolBarLayout {
        ToolButton {
            id: button1
            flat: true
            enabled: window.addMediaState!=UI.ProgressState.Processing
            iconSource: "qrc:/qml/images/back.png"
            onClicked: {
                pageStack.replace(newPostPage)
            }
        }
    }
    Menu {
        id: menu
        property string filepath: ""
        property string filetype: ""
        visualParent: toolBar
        content: MenuLayout {
                MenuItem {
                    text: "Upload"
                    onClicked: {
                        window.addingMediaToPost = true
                        window.addFile(menu.filepath, menu.filetype)
                    }
                }
            }
    }
    Component {
        id: serverMediaDelegate
        Item {
            property bool selected: mouseArea.pressed

            id: wrapper
            visible: fileType==window.ampMediaType
            width: wrapper.ListView.view.width
            height: Math.max(textColumn.childrenRect.height+10, 48)

            Rectangle { id: bgRect; anchors.fill: parent; color: selected?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

            Column {
                id: textColumn
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    margins: 5
                }
                Text {
                    id: titleText
                    text: mediaTitle
                    color: selected?UI.LISTDELEGATE_TITLE_COLOR_MARKED:UI.LISTDELEGATE_TITLE_COLOR
                    width: parent.width
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    font.pixelSize: window.appGeneralFontSize
                }
                Text {
                    text: description
                    color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                    visible: text!=""
                    width: parent.width
                    textFormat: Text.RichText
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    font.pixelSize: titleText.font.pixelSize - 1
                }
                Text {
                    text: caption
                    color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                    visible: text!=""
                    width: parent.width
                    textFormat: Text.RichText
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    font.pixelSize: titleText.font.pixelSize - 1
                }
                Text {
                    text: link
                    color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                    width: parent.width
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    font.pixelSize: titleText.font.pixelSize - 1
                }
                Text {
                    text: dateCreated
                    color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                    width: parent.width
                    textFormat: Text.RichText
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    font.pixelSize: titleText.font.pixelSize - 1
                    font.italic: true
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                enabled: !pageStack.busy
                onClicked:{
                    console.log(newPostPage.mediaItemPostion, newPostPage.postPagetText)
                    window.impClear()
                    window.impThumbSource = link
                    window.impNameText = mediaTitle
                    window.impDescText = description
                    window.impCaptionText = caption
                    window.impTypeText = fileType
                    window.impDateText = dateCreated
                    window.impUrlText = link
                    window.impCaller = window.ampCaller
                    window.replaceFile("InsertMediaPage.qml")
                }
            }
        }
    }
    Component {
        id: localMediaDelegate
        Item {
            property bool selected: mouseArea.pressed

            id: wrapper
            width: wrapper.ListView.view.width
            height: Math.max(itemRow.childrenRect.height+10, 48)

            Rectangle { id: bgRect; anchors.fill: parent; color: selected?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

            Row {
                id: itemRow
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    margins: 5
                }
                spacing: 10

                Image {
                    id: thumb
                    source: fileThumbnail
                    sourceSize.width: 84
                    sourceSize.height: 84
                    visible: status==Image.Ready
                }

                Column {
                    id: textColumn
                    spacing: 5
                    width: parent.width - parent.spacing - thumb.width

                    Text {
                        id: titleText
                        text: fileName
                        color: selected?UI.LISTDELEGATE_TITLE_COLOR_MARKED:UI.LISTDELEGATE_TITLE_COLOR
                        width: parent.width
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        font.pixelSize: window.appGeneralFontSize
                    }
                    Text {
                        id: pathText
                        text: filePath
                        color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                        width: parent.width
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        textFormat: Text.PlainText
                        font.pixelSize: titleText.font.pixelSize - 1
                    }
                    Text {
                        id: sizeText
                        text: fileSize+" mb"
                        color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                        width: parent.width
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        font.pixelSize: titleText.font.pixelSize - 1
                    }
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                enabled: !pageStack.busy && window.addMediaState!=UI.ProgressState.Processing
                onClicked:{
                    console.log(newPostPage.mediaItemPostion, newPostPage.postPagetText)
                    menu.filepath = filePath
                    menu.filetype = fileType
                    menu.open()
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
        BigHeadingText {
            id: heading
            anchors.verticalCenter: parent.verticalCenter
            text: {
                if(window.addMediaState!=UI.ProgressState.Processing || window.addMediaStatus==""){
                    if(window.ampMediaType=="image")
                        return "Select Image"
                    else
                        return "Select Video"
                }else
                    return window.addMediaStatus
            }
            width: status.visible?parent.width-parent.spacing-status.width:parent.width
            elide: Text.ElideRight
            wrapMode: Text.Wrap
        }

        BusyIndicator {
            id: status
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            width: Math.min(heading.height, 24)
            height: width
            running: true
            visible: window.addMediaState==UI.ProgressState.Processing
        }
    }

    TabBar {
        id: tabbar
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            margins: UI.MARGIN_XLARGE
        }
        height: 48
        TabButton { tab: tab1content; text: "Library" }
        TabButton { tab: tab2content; text: "Local" }
    }

    TabGroup {
        anchors.top: tabbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Item{
            id:tab1content
            anchors.fill: parent

            ListView {
                id: serverList
                visible: count!=0
                anchors {
                    fill: parent
                    leftMargin: UI.MARGIN_XLARGE
                    rightMargin: UI.MARGIN_XLARGE
                }
                cacheBuffer: height
                delegate: serverMediaDelegate
                model: mediaModel
                clip: true
                enabled: !pageStack.busy
                spacing: 5
            }

            Text {
                text: "Media items for blogs not hosted on Wordpress.com cannot be fetched"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                width: 0.9*parent.width
                color: "#CCCCCC"
                anchors.centerIn: parent
                font.pixelSize: window.appGeneralFontSize+5
                visible: serverList.count == 0 && !window.isCurrentBlogWPHosted
            }
        }

        ListView {
            id:tab2content
            anchors {
                fill: parent
                leftMargin: UI.MARGIN_XLARGE
                rightMargin: UI.MARGIN_XLARGE
            }
            cacheBuffer: height
            delegate: localMediaDelegate
            model: localMediaModel
            clip: true
            enabled: !pageStack.busy
            spacing: 5
        }

        BusyIndicator {
            id: busyInd
            width: 64
            height: width
            running: true
            visible: !tab2content.visible
            anchors.centerIn: tab2content
        }

        Text {
            text: window.searchMediaStatus
            color: UI.PROGRESSSTATUS_LABEL_COLOR
            anchors.top: busyInd.bottom
            anchors.topMargin: 25
            anchors.horizontalCenter: busyInd.horizontalCenter
            font.pixelSize: window.appGeneralFontSize+15
            visible: busyInd.visible
        }
    }
}

//![0]
