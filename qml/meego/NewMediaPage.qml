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
    id: nMediaPage

    property bool marking: false

    tools: ToolBarLayout {
        ToolIcon {
            id: button1
            enabled: window.addMediaState!=UI.ProgressState.Processing
            iconSource: UI.BACKIMG
            onClicked: {
                pageStack.pop()
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
                    text: qsTr("Upload")
                    onClicked: {
                        window.addFile(menu.filepath, menu.filetype)
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
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                width: parent.width - 10
                spacing: 10

                Image {
                    id: thumb
                    source: fileThumbnail
                    sourceSize.width: 84
                    sourceSize.height: 84
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
                id :mouseArea
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

    Item {
        id: header
        height: headerRow.height+UI.MARGIN_XLARGE
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: UI.MARGIN_XLARGE
        }
        Row {
            id: headerRow
            spacing: 5
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottomMargin: UI.MARGIN_XLARGE
            }

            BigHeadingText {
                id: heading
                anchors.verticalCenter: parent.verticalCenter
                text: {
                    if(window.addMediaState!=UI.ProgressState.Processing || window.addMediaStatus==""){
                        if(window.nmpMediaType=="image")
                            return qsTr("Select Image")
                        else
                            return qsTr("Select Video")
                    }else
                        return window.addMediaStatus
                }
                width: parent.width-parent.spacing-status.width
                elide: Text.ElideRight
                wrapMode: Text.Wrap
            }

            BusyIndicator {
                id: status
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                platformStyle: BusyIndicatorStyle { size: "small" }
                running: true
                visible: window.addMediaState==UI.ProgressState.Processing
            }
        }
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: UI.PAGE_HEADER_SEPARATOR
        }
    }

    ListView {
        id:localMediaList
        anchors {
            top: header.bottom
            topMargin: UI.MARGIN_XLARGE
            left: parent.left
            leftMargin: UI.MARGIN_XLARGE
            right: parent.right
            rightMargin: UI.MARGIN_XLARGE
            bottom: parent.bottom
        }
        cacheBuffer: height
        delegate: localMediaDelegate
        model: localMediaModel
        clip: true
        enabled: !pageStack.busy
        spacing: 5
        visible: window.searchMediaState==UI.ProgressState.Success
        onMovingChanged: {
            if(atYEnd && window.searchMediaState!=UI.ProgressState.Processing){
                window.writeMediaItemsToModel(count)
            }
        }
    }

    BusyIndicator {
        id: busyInd
        platformStyle: BusyIndicatorStyle { size: "large" }
        running: true
        visible: !localMediaList.visible
        anchors.centerIn: localMediaList
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

//![0]
