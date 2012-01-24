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
import com.nokia.extras 1.0
import "../UIConstants.js" as UI

CPPage {
    id: postPage

    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
    tools: ToolBarLayout {
        ToolButton {
            id: button1
            flat: true
            iconSource: "qrc:/qml/images/back.png"
            onClicked: {
                commentList.height = 0
                pageFlickable.contentHeight = 0
                pageStack.pop()
            }
        }
        ToolButton {
            id: button2
            flat: true
            iconSource: "qrc:/qml/images/recycle bin.png"
            onClicked: showDialog("Doy you really want to delete post \"<strong>"+window.sppPostTitle+"</strong>\"?")
        }
    }

    QueryDialog {
        id: query
        acceptButtonText: "Yes"
        rejectButtonText: "No"
        titleText: "Delete post"
        onAccepted: window.deletePost(window.sppPostId)
    }

    function showDialog(msg){
        query.message = msg
        query.open()
    }

    Flickable {
        id: pageFlickable
        anchors {
            fill: parent
            margins: UI.MARGIN_XLARGE
        }
        contentWidth: width
        contentHeight: content.height
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

            Item {
                id: postHeader
                width: parent.width
                height: postHeaderColumn.childrenRect.height+35

                Column {
                    id: postHeaderColumn
                    width: parent.width
                    anchors.top: parent.top
                    spacing: 2

                    ListSectionDelegate {
                        id: browsingSection
                        anchors.rightMargin: 0
                        anchors.leftMargin: 0
                        margins: 0
                        sectionName: "Post"
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: pStatus.height+4
                        color: {
                            if(pStatus.text == "Published")
                                return "green";
                            else if(pStatus.text == "Private")
                                return "lightgreen";
                            else if(pStatus.text == "Scheduled in future")
                                return "#5674b9";
                            else if(pStatus.text == "Pending review")
                                return "yellow";
                            else if(pStatus.text == "Draft"||pStatus.text == "Local Draft" || pStatus.text == "Busy")
                                return "orange";
                            else if(pStatus.text == "Trashed"||pStatus.text == "Orphaned")
                                return "red";
                        }

                        Text {
                            id: pStatus
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

                    BigHeadingText {
                        text: {
                            var txt = "<strong>" + window.sppPostTitle + "</strong>"
                            if(window.sppPostPassword!="")
                                txt = "[Protected] <strong>" + window.sppPostTitle + "</strong>"
                            return txt
                        }
                        font.pixelSize: window.appGeneralFontSize+2
                        wrapMode: Text.Wrap
                        width: parent.width
                        onTextChanged: pageFlickable.contentY = 0
                    }

                    Text {
                        text: "by "+window.sppPostAuthor
                        visible: window.sppPostAuthor!=""
                        textFormat: Text.RichText
                        font.pixelSize: window.appGeneralFontSize - 1
                        color: UI.LISTDELEGATE_TEXT_COLOR1
                    }

                    Text {
                        text: "Categorized in <em>"+window.sppPostCategories+"</em>"
                        textFormat: Text.RichText
                        wrapMode: Text.Wrap
                        font.pixelSize: window.appGeneralFontSize - 1
                        color: UI.LISTDELEGATE_TEXT_COLOR
                        width: parent.width
                        visible: window.sppPostCategories!=""
                    }

                    Text {
                        text: "on "+window.sppPostDate
                        visible: window.sppPostDate!=""
                        textFormat: Text.RichText
                        font.pixelSize: window.appGeneralFontSize - 1
                        color: UI.LISTDELEGATE_TEXT_COLOR1
                    }
                }
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 25
                    height: 1
                    color: "#CCCCCC"
                }
            }
            Text {
                id: con
                text: window.sppPostHtml
                width: Math.max(0,parent.width)
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                color: UI.HEADINGTEXT_COLOR
                font.pixelSize: window.appGeneralFontSize+1
            }
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 30
            }

            HeadingText {
                text: "Add reply"
                color: UI.PAGE_HEADER_TITLE_COLOR
                font.bold: false
                visible: window.sppPostPublishStatus!="Local Draft"
            }

            TextArea {
                id: addCommentField
                enabled: window.addCommentState != UI.ProgressState.Processing
                width: parent.width
                font.pixelSize: window.appInputFontSize
                placeholderText: "Enter your reply here"
                height: Math.max (100, implicitHeight)
                visible: window.sppPostPublishStatus!="Local Draft"
            }

            Row{
                anchors.left: parent.left
                anchors.right: parent.right
                visible: window.sppPostPublishStatus!="Local Draft"

                BusyIndicator {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    width: 24
                    height: 24
                    visible: window.addCommentState == UI.ProgressState.Processing
                    running: true
                }

                Button{
                    text: "Post"
                    enabled: window.addCommentState != UI.ProgressState.Processing
                    onClicked: window.addComment(window.sppPostId, addCommentField.text)
                    font.pixelSize: window.appInputFontSize
                    width: 0.25*parent.width
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            HeadingText {
                id: commentsHeading
                text: commentList.count!=0?"Comments("+commentList.count+")":"No comments yet"
                color: UI.PAGE_HEADER_TITLE_COLOR
                font.bold: false
                visible: window.sppPostPublishStatus!="Local Draft"
            }

            Component {
                id: commentDelegate
                Item {
                    id: wrapper

                    property bool isOpen: commentList.selectedComment == index

                    width: wrapper.ListView.view.width
                    height: Math.max(contentRow.childrenRect.height+contentRow.anchors.margins*2, 64)


                    Rectangle { anchors.fill: parent; color: UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

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
                                font.pixelSize: titleText.font.pixelSize-1
                                elide: Text.ElideRight
                                color: UI.LISTDELEGATE_TEXT_COLOR1
                                font.italic: true
                                horizontalAlignment: Text.AlignRight
                            }

                            Text {
                                id: titleText
                                text: authorName+" on <strong>"+postTitle+"</strong>"
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
                                color: UI.LISTDELEGATE_TEXT_COLOR1
                            }

                            Text {
                                text: authorUrl
                                visible: authorUrl!=""
                                width: parent.width
                                font.pixelSize: titleText.font.pixelSize-1
                                elide: Text.ElideRight
                                color: UI.LISTDELEGATE_TEXT_COLOR1
                            }

                            Text {
                                text: authorIp
                                width: parent.width
                                visible: authorIp!=""
                                font.pixelSize: titleText.font.pixelSize-1
                                elide: Text.ElideRight
                                color: UI.LISTDELEGATE_TEXT_COLOR1
                            }

                            Text {
                                text: content
                                textFormat: Text.RichText
                                wrapMode: Text.Wrap
                                width: parent.width
                                font.pixelSize: titleText.font.pixelSize
                                color: UI.LISTDELEGATE_TEXT_COLOR
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
                        onPressAndHold: commentList.showMenu(index, localId, postId, commentStatus)
                    }
                }
            }

            ListView {
                id: commentList
                width: parent.width
                height: Math.max(5, contentHeight)
                cacheBuffer: height
                delegate: commentDelegate
                model: postCommentsModel
                clip: true
                enabled: !pageStack.busy
                spacing: 5
                interactive: false
                visible: window.sppPostPublishStatus!="Local Draft"
                property int selectedComment: -1

                ContextMenu {
                    id: contextMenu
                    visualParent: commentList
                    property int index: -1
                    property string commentId: ""
                    property string commentsPostId: ""
                    property string commentStatus: ""
                    content:  MenuLayout {
                        MenuItem {
                            text:{
                                if(contextMenu.commentStatus=="hold")
                                    return "Approve";
                                else if(contextMenu.commentStatus=="approve")
                                    return "Unapprove";
                            }
                            onClicked: {
                                if(contextMenu.commentStatus=="hold")
                                    window.approveComment(contextMenu.commentId);
                                else if(contextMenu.commentStatus=="approve")
                                    window.unApproveComment(contextMenu.commentId);
                            }
                        }
                        MenuItem { text: "Delete"; onClicked: { window.deleteComment(contextMenu.commentId) } }
                        MenuItem { text: "Spam"; onClicked: { window.spamComment(contextMenu.commentId) } }
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
    }


}
//![0]
