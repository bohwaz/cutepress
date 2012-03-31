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
    id: blogsPage


    function openCurrentBlog(){
        console.log("Oopen Current")
        if(!pageStack.busy){
            pageStack.replace(blogOptionsPage)
        }
        else
            console.log("Pagestack Buusy")
    }

    tools: ToolBarLayout {
        ToolIcon {
            id: button1
            iconSource: window.isThemeInverted?UI.ADDIMG:UI.ADDIMG
            onClicked: {
                window.nbpClear()
                window.openFile("NewBlogPage.qml")
            }
        }
        ToolIcon {
            id: button4
            iconSource: window.isThemeInverted?UI.MENUIMG:UI.MENUIMG
            onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }/*

    Menu {
        id: myMenu
        visualParent: blogsPage
        MenuLayout {
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
            MenuItem { text: qsTr("Exit"); onClicked: Qt.quit(); }
        }
    }*/

    Item {
        id: header
        height: heading.height+10
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: UI.MARGIN_XLARGE
        }

        BigHeadingText {
            id: heading
            text: qsTr("Blogs")
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
        }
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
        }
    }

    Component {
        id: blogDelegate
        Item {
            property bool selected: mouseArea.pressed

            id: wrapper
            width: wrapper.ListView.view.width
            anchors.margins: 0
            height: Math.max(contentRow.childrenRect.height, 25)

            Rectangle {
                id: bgRect
                anchors.fill: parent
                color: selected?
                           window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:
                           window.isThemeInverted?UI.LISTDELEGATE_WRAPPER_COLOR:UI.LISTDELEGATE_WRAPPER_COLOR
                opacity: 0.8
            }

            Row {
                id: contentRow
                anchors.margins: 0
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: UI.LISTDELEGATE_SPACING
                Item {
                    height: textColumn.childrenRect.height+textColumn.anchors.margins
                    width: parent.width - parent.spacing - buttonColumn.width
                    anchors.verticalCenter: parent.verticalCenter

                    Column {
                        id: textColumn
                        anchors.margins: 5
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: UI.LISTDELEGATE_SPACING

                        Text {
                            id: titleText
                            text: title
                            color: selected?UI.LISTDELEGATE_TITLE_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TITLE_COLOR:UI.LISTDELEGATE_TITLE_COLOR
                            width: parent.width
                            font.bold: true
                            textFormat: Text.RichText
                            elide: Text.ElideRight
                            wrapMode: Text.Wrap
                            font.pixelSize: window.appGeneralFontSize
                        }

                        Text {
                            text: (isAdmin)?qsTr("<b>%1</b> | Admin").arg(username):qsTr("<b>%1</b> | User").arg(username)
                            width: parent.width
                            font.pixelSize: titleText.font.pixelSize-1
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR1:UI.LISTDELEGATE_TEXT_COLOR1
                        }

                        Text {
                            text: (pagesCount==''&&postsCount==''&&commentsCount=='')?qsTr("Blog Data Not Available"):qsTr("%1 page(s) | %2 post(s) | %3 comment(s)").arg(pagesCount).arg(postsCount).arg(commentsCount)
                            width: parent.width
                            font.pixelSize: titleText.font.pixelSize-1
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:(pagesCount==''&&postsCount==''&&commentsCount=='')?UI.LISTDELEGATE_TEXT_COLOR:window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR1:UI.LISTDELEGATE_TEXT_COLOR1
                        }

                        Text {
                            text: qsTr("Hosted at WordPress.com")
                            width: parent.width
                            font.pixelSize: titleText.font.pixelSize-1
                            font.italic: true
                            visible: isWordpress
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            window.openBlog(blogId)
                            pageStack.push(blogOptionsPage)
                        }
                        onPressAndHold: console.log("pages",pagesCount,"pages",postsCount,"comments",commentsCount)
                    }
                }

                Column {
                    id: buttonColumn
                    width: editButton.width
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 10

                    ToolIcon {
                        id: editButton
                        height: 45
                        iconSource: selected?"qrc:/qml/images/edit - white.png":window.isThemeInverted?UI.EDITIMG:UI.EDITIMG
                        onClicked: {
                            window.nbpClear()
                            window.nbpIsEditingBlog = true
                            window.nbpBlogId = blogId
                            window.nbpUrlText = blogUrl
                            window.nbpUsernameText = username
                            window.nbpPasswordText = password
                            window.nbpNumberOfPosts = blogNumberOfPosts
                            window.openFile("NewBlogPage.qml")
                        }
                    }
                    ToolIcon {
                        id: deleteButton
                        height: 45
                        iconSource: selected?"qrc:/qml/images/close - white.png":window.isThemeInverted?UI.CLOSEIMG:UI.CLOSEIMG
                        onClicked: deleteBlog(blogId)
                    }
                }
            }
        }
    }


    ListView {
        id:blogList
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
        delegate: blogDelegate
        model: blogsModel
        clip: true
        spacing: 5
        enabled: !pageStack.busy
        onCountChanged: console.log("sasasasa")
        interactive: contentHeight>height
        Component.onCompleted: console.log(count)
    }

    Text {
        text: qsTr("No Blog Found")
        color: window.isThemeInverted?UI.PROGRESSSTATUS_LABEL_COLOR:UI.PROGRESSSTATUS_LABEL_COLOR
        anchors.centerIn: blogList
        font.pixelSize: window.appGeneralFontSize+15
        visible: blogList.count == 0
    }

}
//![0]
