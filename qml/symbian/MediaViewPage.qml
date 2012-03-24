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
            iconSource: window.isIconsMetro?"qrc:/qml/images/back.png":"qrc:/qml/images/symbian/symbian_back.png"
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
            iconSource:"qrc:/qml/images/mark.png"
            onClicked: {
                marking = true
            }
        }*/
        ToolButton {
            id: button2
            iconSource: window.isIconsMetro?"qrc:/qml/images/reload.png":"qrc:/qml/images/symbian/symbian_reload.png"
            visible: !status.visible
            onClicked: window.refreshCurrentBlogMedia()
        }
        ToolButton {
            id: button3
            flat: true
            iconSource: window.isIconsMetro?"qrc:/qml/images/add.png":"qrc:/qml/images/symbian/symbian_add.png"
            onClicked: menu.open()
        }
    }
    Menu {
        id: menu
        visualParent: toolBar
        content: MenuLayout {
                MenuItem {
                    text: qsTr("New Image")
                    onClicked: {
                        window.searchMedia("image")
                        window.nmpMediaType = "image"
                        window.openFile("NewMediaPage.qml")
                    }
                }
                MenuItem {
                    text: qsTr("New Video")
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
        height: headerRow.childrenRect.height+UI.MARGIN_XLARGE
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
            }

            BigHeadingText {
                id: heading
                anchors.verticalCenter: parent.verticalCenter
                text: {
                    if(window.mediaModelStatus=="")
                        return qsTr("Media Items")
                    else
                        return window.mediaModelStatus
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
                visible: window.mediaModelState==UI.ProgressState.Processing
            }
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
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
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    margins: 5
                }
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
                    font.pixelSize: titleText.font.pixelSize - 2
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
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
    }

    ScrollDecorator {
        flickableItem: mediaList
    }
}
//![0]
