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

//    property int myCurrentItem: -1
    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }


    function openCurrentBlog(){
        console.log("Oopen Current")
        if(!pageStack.busy){
            pageStack.replace(blogOptionsPage)
        }
        else
            console.log("Pagestack Buusy")
    }

    tools: ToolBarLayout {
        ToolButton {
            id: button1
            flat: true
            iconSource: "qrc:/qml/images/add.png"
            onClicked: {
                window.nbpClear()
                window.openFile("NewBlogPage.qml")
            }
        }
        ToolButton {
            id: button4
            flat: true
            iconSource: "qrc:/qml/images/menu.png"
            onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    Item {
        id: header
        height: heading.height+UI.MARGIN_XLARGE
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: UI.MARGIN_XLARGE
        }

        BigHeadingText {
            id: heading
            text: "Blogs"
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 1
            color: window.isThemeInverted?UI.PAGE_HEADER_SEPARATOR:UI.PAGE_HEADER_SEPARATOR
        }
    }

    Component {
        id: blogDelegate
        Item {
            property bool selected: mouseArea.pressed

            id: wrapper
            width: wrapper.ListView.view.width
            anchors.margins: 0
            height: Math.max(contentRow.childrenRect.height, 82)

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
                anchors {
                    margins: 0
                    left: parent.left
                    right: parent.right
                }
                spacing: UI.LISTDELEGATE_SPACING
                Item {
                    height: textColumn.childrenRect.height+textColumn.anchors.margins
                    width: parent.width - parent.spacing - buttonColumn.width
                    anchors.verticalCenter: parent.verticalCenter

                    Column {
                        id: textColumn
                        anchors {
                            margins: 5
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
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
                            text: (isAdmin)?"<b>"+username+"</b> | Admin":"<b>"+username+"</b> | User"
                            width: parent.width
                            font.pixelSize: titleText.font.pixelSize-1
                            //visible: (pagesCount!=''||pagesCount=='0')&&postsCount!=''&&commentsCount!=''
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR1:UI.LISTDELEGATE_TEXT_COLOR1
                        }

                        Text {
                            text: (pagesCount==''&&postsCount==''&&commentsCount=='')?"Blog Data Not Available":pagesCount+" page(s) | "+postsCount+" post(s) | "+commentsCount+" comment(s)"
                            width: parent.width
                            font.pixelSize: titleText.font.pixelSize-1
                            //visible: (pagesCount!=''||pagesCount=='0')&&postsCount!=''&&commentsCount!=''
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:
                                            (pagesCount==''&&postsCount==''&&commentsCount=='')?
                                                    window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR:
                                                    window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR1:UI.LISTDELEGATE_TEXT_COLOR1
                        }

                        Text {
                            text: "Hosted at WordPress.com"
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
                    }
                }

                Column {
                    id: buttonColumn
                    width: editButton.width
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 10

                    ToolButton{
                        id: editButton
                        flat: true
                        width: 36; height: 36
                        iconSource: selected?"qrc:/qml/images/edit - white.png":window.isThemeInverted?UI.EDITIMG:UI.EDITIMG
                        onClicked: {
                            window.nbpClear()
                            window.nbpIsEditingBlog = true
                            window.nbpBlogId = blogId
                            window.nbpUrlText = blogUrl
                            window.nbpUsernameText = username
                            window.nbpPasswordText = password
                            window.openFile("NewBlogPage.qml")
                        }
                    }
                    ToolButton{
                        id: deleteButton
                        flat: true
                        width: 36; height: 36
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
        Component.onCompleted: console.log(count)
    }

    Text {
        text: "No Blog Found"
        color: window.isThemeInverted?UI.PROGRESSSTATUS_LABEL_COLOR:UI.PROGRESSSTATUS_LABEL_COLOR
        anchors.centerIn: blogList
        font.pixelSize: window.appGeneralFontSize+5
        visible: blogList.count == 0
    }

}
//![0]
