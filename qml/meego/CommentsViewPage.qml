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
    id: commentsPage

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
            iconSource:"qrc:/qml/images/mark.png"
            onClicked: {
                marking = true
            }
        }*/
        ToolIcon {
            id: button2
            iconSource: UI.RELOADIMG
            visible: !status.visible
            onClicked: window.refreshCurrentBlogComments()
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
                if(window.commentModelStatus=="")
                    return qsTr("Comments")
                else
                    return window.commentModelStatus
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
            visible: window.commentModelState==UI.ProgressState.Processing
        }

            /*ToolButton {
                id: refreshButton
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                width: 32
                height: width
                flat: true
                iconSource: UI.RELOADIMG
                visible: !status.visible
                enabled: !pageStack.busy
                onClicked: window.refreshCurrentBlogComments()
            }*/
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
        id: commentDelegate
        Item {
            id: wrapper

            property bool isOpen: commentList.selectedComment == index

            width: wrapper.ListView.view.width
            height: Math.max(contentRow.childrenRect.height+contentRow.anchors.margins*2, 64)


            Rectangle { anchors.fill: parent; color: isOpen?window.isThemeInverted?"#4b4b4b":"#b7b7b7":UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

            Row {
                id: contentRow
                anchors.margins: 5
                width: wrapper.width
                spacing: 8
                anchors.fill: parent

                Image {
                    id: gravatar
                    source: gravatarUrl
                    sourceSize.width: 64
                    sourceSize.height: 64
                    smooth: true
                }

                Column {
                    id: textColumn
                    spacing: UI.LISTDELEGATE_SPACING
                    width: contentRow.width - gravatar.width - 2*contentRow.spacing - 5

                    Text {
                        text: dateCreated;
                        width: parent.width
                        font.pixelSize: titleText.font.pixelSize-2
                        elide: Text.ElideRight
                        color: UI.LISTDELEGATE_TEXT_COLOR
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        id: titleText
                        text: qsTr("%1 on <strong>%2</strong>").arg(authorName).arg(postTitle)
                        color: UI.LISTDELEGATE_TITLE_COLOR
                        width: parent.width
                        textFormat: Text.RichText
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                        font.pixelSize: window.appGeneralFontSize
                    }

                    Text {
                        text: authorEmail
                        visible: authorEmail!=""
                        width: parent.width
                        font.pixelSize: titleText.font.pixelSize-1
                        elide: Text.ElideRight
                        color: UI.LISTDELEGATE_TEXT_COLOR
                    }

                    Text {
                        text: authorUrl
                        visible: authorUrl!=""
                        width: parent.width
                        font.pixelSize: titleText.font.pixelSize-1
                        elide: Text.ElideRight
                        color: UI.LISTDELEGATE_TEXT_COLOR
                    }

                    Text {
                        text: authorIp;
                        width: parent.width
                        visible: authorIp!=""
                        font.pixelSize: titleText.font.pixelSize-1
                        elide: Text.ElideRight
                        color: UI.LISTDELEGATE_TEXT_COLOR
                    }

                    Text {
                        text: isOpen?content:preview;
                        textFormat: Text.RichText
                        wrapMode: Text.Wrap
                        width: parent.width
                        font.pixelSize: titleText.font.pixelSize
                        color: UI.LISTDELEGATE_TEXT_COLOR1
                    }
                }
                Rectangle {
                    width: 5
                    height: textColumn.childrenRect.height
                    color: (commentStatus=="approve")?"green":"orange"
                    anchors.right: parent.right
                }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(isOpen)
                        commentList.selectedComment = -1
                    else
                        commentList.selectedComment = index
                }
                onPressAndHold: {
                    commentList.selectedComment = index
                    commentList.showMenu(index, localId, postId, commentStatus)
                }
            }
        }
    }

    ListView {
        id:commentList        
        property int selectedComment: -1
        anchors {
            top: header.bottom
            topMargin: UI.MARGIN_XLARGE
            left: parent.left
            leftMargin: UI.MARGIN_XLARGE
            right: parent.right
            rightMargin: UI.MARGIN_XLARGE
            bottom: parent.bottom
            margins: UI.LISTVIEW_MARGIN
        }
        cacheBuffer: 2 * height
        delegate: commentDelegate
        model: commentsModel
        clip: true
        enabled: !pageStack.busy
        spacing: 5

        onCountChanged: {
            if(contextMenu.index<count)
                positionViewAtIndex(contextMenu.index, ListView.Contain)
        }

        ContextMenu {
            id: contextMenu
            visualParent: commentsPage
            property int index: -1
            property string commentId: ""
            property string commentsPostId: ""
            property string commentStatus: ""
            content:  MenuLayout {
                MenuItem {
                    text:{
                        if(contextMenu.commentStatus=="hold")
                            return qsTr("Approve");
                        else if(contextMenu.commentStatus=="approve")
                            return qsTr("Unapprove");
                    }
                    onClicked: {
                        if(contextMenu.commentStatus=="hold")
                            window.approveComment(contextMenu.commentId);
                        else if(contextMenu.commentStatus=="approve")
                            window.unApproveComment(contextMenu.commentId);
                    }
                }
                MenuItem { text: qsTr("Reply"); onClicked: { window.replyToComment(contextMenu.commentId, contextMenu.commentsPostId) } }
                MenuItem { text: qsTr("Delete"); onClicked: { window.deleteComment(contextMenu.commentId) } }
                MenuItem { text: qsTr("Spam"); onClicked: { window.spamComment(contextMenu.commentId) } }
            }
        }

        function showMenu(index, id, pId, status){
            contextMenu.index = index
            contextMenu.commentId = id
            contextMenu.commentsPostId = pId
            contextMenu.commentStatus = status
            contextMenu.open()
        }
    }
}
//![0]
