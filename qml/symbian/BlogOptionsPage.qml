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

    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
    tools: ToolBarLayout {
        ToolButton {
            id: button1
            flat: true
            iconSource: "qrc:/qml/images/blogs.png"
            onClicked: {
                pageStack.pop()
                console.log(toolBar.visible,toolBar.enabled)
            }
        }
        ToolButton {
            id: button2
            flat: true
            visible: !status.visible
            iconSource: "qrc:/qml/images/reload.png"
            onClicked: window.refreshCurrentBlog()
        }
        ToolButton {
            id: button3
            flat: true
            iconSource: "qrc:/qml/images/menu.png"
            onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
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
            text: window.currentBlogName
            width: parent.width-parent.spacing-status.width
            textFormat: Text.RichText
            wrapMode: Text.Wrap
            color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
        }

        BusyIndicator {
            id: status
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            width: Math.min(heading.height, 24)
            height: width
            running: true
            visible: window.pageModelState==UI.ProgressState.Processing||
                     window.postModelState==UI.ProgressState.Processing||
                     window.commentModelState==UI.ProgressState.Processing||
                     window.mediaModelState==UI.ProgressState.Processing
        }
    }

    Flickable {
        id: pageFlickable
        clip: true
        anchors{
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: UI.MARGIN_XLARGE
        }
        contentHeight: optionsColumn.childrenRect.height+2*UI.MARGIN_XLARGE
        flickableDirection: Flickable.VerticalFlick
        interactive: contentHeight>height

        Column{
            id: optionsColumn
            anchors.fill: parent
            spacing: UI.LISTDELEGATE_SPACING+20

            Item {
                width: parent.width
                height: 48

                Rectangle { anchors.fill: parent; color: window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.85; visible: mouseArea1.pressed}

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    Text {
                        text: "Pages"; color: window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR;
                        font.pixelSize: window.appGeneralFontSize + 1
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 36; height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        radius: 6
                        color: window.isThemeInverted?UI.BLOG_ITEMCOUNT_RECT:UI.BLOG_ITEMCOUNT_RECT
                        Text {
                            text: {
                                if(window.currentBlogPagesCount>99)
                                    return "99+"
                                else
                                    return window.currentBlogPagesCount
                            }
                            color: "white"
                            font.pixelSize: window.appGeneralFontSize - 2
                            anchors.centerIn: parent
                        }
                    }
                }

                MouseArea {
                    id: mouseArea1
                    anchors.fill: parent
                    enabled: !pageStack.busy
                    onClicked: openFile("PagesViewPage.qml")
                }
            }

            Item {
                width: parent.width
                height: 48

                Rectangle { anchors.fill: parent; color: window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.85; visible: mouseArea2.pressed}

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    Text {
                        text: "Posts"; color: window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR;
                        font.pixelSize: window.appGeneralFontSize + 1
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 36; height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        radius: 6
                        color: window.isThemeInverted?UI.BLOG_ITEMCOUNT_RECT:UI.BLOG_ITEMCOUNT_RECT
                        Text {
                            text: {
                                if(window.currentBlogPostsCount>99)
                                    return "99+"
                                else
                                    return window.currentBlogPostsCount
                            }
                            color: "white"
                            font.pixelSize: window.appGeneralFontSize - 2
                            anchors.centerIn: parent
                        }
                    }
                }

                MouseArea {
                    id: mouseArea2
                    anchors.fill: parent
                    enabled: !pageStack.busy
                    onClicked: openFile("PostsViewPage.qml")
                }
            }

            Item {
                width: parent.width
                height: 48

                Rectangle { anchors.fill: parent; color: window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.85; visible: mouseArea3.pressed}

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    Text {
                        text: "Comments"; color: window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR;
                        font.pixelSize: window.appGeneralFontSize + 1
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 36; height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        radius: 6
                        color: window.isThemeInverted?UI.BLOG_ITEMCOUNT_RECT:UI.BLOG_ITEMCOUNT_RECT
                        Text {
                            text: {
                                if(window.currentBlogCommentsCount>99)
                                    return "99+"
                                else
                                    return window.currentBlogCommentsCount
                            }
                            color: "white"
                            font.pixelSize: window.appGeneralFontSize - 2
                            anchors.centerIn: parent
                        }
                    }
                }

                MouseArea {
                    id: mouseArea3
                    anchors.fill: parent
                    enabled: !pageStack.busy
                    onClicked: openFile("CommentsViewPage.qml")
                }
            }

            Item {
                width: parent.width
                height: 48
                visible: window.isCurrentBlogWPHosted

                Rectangle { anchors.fill: parent; color: window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.85; visible: mouseArea4.pressed}

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    Text {
                        text: "Media"; color: window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR;
                        font.pixelSize: window.appGeneralFontSize + 1
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle {
                        width: 36; height: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        radius: 6
                        color: window.isThemeInverted?UI.BLOG_ITEMCOUNT_RECT:UI.BLOG_ITEMCOUNT_RECT
                        Text {
                            text: {
                                if(window.currentBlogMediaCount>99)
                                    return "99+"
                                else
                                    return window.currentBlogMediaCount
                            }
                            color: "white"
                            font.pixelSize: window.appGeneralFontSize - 2
                            anchors.centerIn: parent
                        }
                    }
                }

                MouseArea {
                    id: mouseArea4
                    anchors.fill: parent
                    enabled: !pageStack.busy
                    onClicked: openFile("MediaViewPage.qml")
                }
            }
        }
    }
}
//![0]
