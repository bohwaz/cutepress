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
    id: mediaPage

    property bool marking: false

    tools: ToolBarLayout {
        ToolIcon {
            id: button1
            iconSource: UI.BACKIMG
            onClicked: {
                if(!marking)
                    pageStack.pop()
                else
                    marking = !marking
            }
        }
        /*ToolButton {
            id: button2
            flat: true
            visible: !marking
            iconSource: "qrc:/qml/images/mark.png"
            onClicked: {
                marking = true
            }
        }*/
        ToolIcon {
            id: button2
            iconSource: UI.RELOADIMG
            visible: !status.visible
            onClicked: window.refreshCurrentBlogMedia()
        }
        ToolIcon {
            id: button3
            iconSource: UI.ADDIMG
            onClicked: menu.open()
        }
    }
    Menu {
        id: menu
        visualParent: toolBar
        content: MenuLayout {
                MenuItem {
                    text: "New Image"
                    onClicked: {
                        window.searchMedia("image")
                        window.nmpMediaType = "image"
                        window.openFile("NewMediaPage.qml")
                    }
                }
                MenuItem {
                    text: "New Video"
                    onClicked: {
                        window.searchMedia("video")
                        window.nmpMediaType = "video"
                        window.openFile("NewMediaPage.qml")
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
                    if(window.mediaModelStatus=="")
                        return "Media Items"
                    else
                        return window.mediaModelStatus
                }
                width: parent.width-parent.spacing-status.width
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                color: UI.PAGE_HEADER_TITLE_COLOR
            }

            BusyIndicator {
                id: status
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                platformStyle: BusyIndicatorStyle { size: "small" }
                running: true
                visible: window.mediaModelState==UI.ProgressState.Processing
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

    Component {
        id: mediaDelegate
        Item {
            property bool selected: mouseArea.pressed

            id: wrapper
            width: wrapper.ListView.view.width
            height: Math.max(textColumn.childrenRect.height+10, 48)

            Rectangle { id: bgRect; anchors.fill: parent; color: selected?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

            Column {
                id: textColumn
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                width: parent.width - 10
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
                    color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR1
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
//                onPressed: myCurrentItem = index
//                onReleased: myCurrentItem = -1
//                onFocusChanged: {
//                    if(!focus)
//                        myCurrentItem = -1
//                }
                onClicked: {
                    window.smpClear()
                    window.smpThumbSource = link
                    window.smpNameText = mediaTitle
                    window.smpDescText = description
                    window.smpCaptionText = caption
                    window.smpTypeText = fileType
                    window.smpDateText = dateCreated
                    window.smpUrlText = link
                    window.openFile("SingleMediaPage.qml")
                }
            }
        }
    }

    ListView {
        id:mediaList
        anchors {
            top: header.bottom
            topMargin: UI.MARGIN_XLARGE
            left: parent.left
            leftMargin: UI.MARGIN_XLARGE
            right: parent.right
            rightMargin: UI.MARGIN_XLARGE
            bottom: parent.bottom
        }
        cacheBuffer: 2 * height
        delegate: mediaDelegate
        model: mediaModel
        clip: true
        enabled: !pageStack.busy
        spacing: 5
//        onMovementStarted: myCurrentItem = -1
    }
}
//![0]
