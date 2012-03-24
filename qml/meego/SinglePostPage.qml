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
    id: postPage

    tools: ToolBarLayout {
        ToolIcon {
            id: button1
            iconSource: UI.BACKIMG
            onClicked: {
                commentList.height = 0
                pageFlickable.contentHeight = 0
                content.childrenRect.height = 0
                pageStack.pop()
            }
        }
        ToolIcon {
            id: button2
            iconSource: UI.DELETEBIGIMG
            onClicked: showDialog(qsTr("Doy you really want to delete post \"<strong>%1</strong>\"?").arg("+window.sppPostTitle+"))
        }
    }

    QueryDialog {
        id: query
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        titleText: qsTr("Delete post")
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
                        sectionName: qsTr("Post")
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: pStatus.height+4
                        color: {
                            if(pStatus.text == qsTr("Published"))
                                return "green";
                            else if(pStatus.text == qsTr("Private"))
                                return "lightgreen";
                            else if(pStatus.text == qsTr("Scheduled in future"))
                                return "#5674b9";
                            else if(pStatus.text == qsTr("Pending review"))
                                return "yellow";
                            else if(pStatus.text == qsTr("Draft")||pStatus.text == qsTr("Local Draft") || pStatus.text == qsTr("Busy"))
                                return "orange";
                            else if(pStatus.text == qsTr("Trashed")||pStatus.text == qsTr("Orphaned"))
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
                            var txt = qsTr("<strong>%1</strong>").arg(window.sppPostTitle)
                            if(window.sppPostPassword!="")
                                txt = qsTr("[Protected] <strong>%1</strong>").arg(window.sppPostTitle)
                            return txt
                        }
                        font.pixelSize: window.appGeneralFontSize+2
                        wrapMode: Text.Wrap
                        width: parent.width
                        onTextChanged: pageFlickable.contentY = 0
                    }

                    Text {
                        text: qsTr("by %1").arg(window.sppPostAuthor)
                        visible: window.sppPostAuthor!=""
                        textFormat: Text.RichText
                        font.pixelSize: window.appGeneralFontSize - 1
                        color: UI.LISTDELEGATE_TEXT_COLOR1
                    }

                    Text {
                        text: qsTr("Categorized in <em>%1</em>").arg(window.sppPostCategories)
                        textFormat: Text.RichText
                        wrapMode: Text.Wrap
                        font.pixelSize: window.appGeneralFontSize - 1
                        color: UI.LISTDELEGATE_TEXT_COLOR
                        width: parent.width
                        visible: window.sppPostCategories!=""
                    }

                    Text {
                        text: qsTr("on %1").arg(window.sppPostDate)
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

            /*Flickable{
                width:parent.width
                height: con.implicitHeight
                contentHeight: con.implicitHeight+20
                contentWidth: Math.max(1024,parent.width)+20
                flickableDirection: Flickable.HorizontalFlick*/


            Text {
                id: con
                text: window.sppPostHtml
                //anchors.fill: parent
                width: Math.max(0,parent.width)
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                color: UI.HEADINGTEXT_COLOR
                //readOnly: true
                font.pixelSize: window.appGeneralFontSize+1
            }
            //}
            /*Flickable{
                id:iflick
                clip: true
                width: parent.width - 20
                height: 400
                contentWidth: Math.max(parent.width,webView.width)
                contentHeight: webView.height

                onWidthChanged : {
                         // Expand (but not above 1:1) if otherwise would be smaller that available width.
                         if (width > webView.width*webView.contentsScale && webView.contentsScale < 1.0)
                             webView.contentsScale = width / webView.width * webView.contentsScale;
                     }


                flickableDirection: Flickable.HorizontalAndVerticalFlick
                WebView {
                    id: webView
                    html: window.sppPostHtml
                    preferredWidth: iflick.width
                    preferredHeight: contentsSize.height*contentsScale
                    contentsScale: 1.0
                    onContentsSizeChanged: {
                        // zoom out
                        contentsScale = Math.min(1,iflick.width / contentsSize.width)
                    }

                    smooth: false
                    onDoubleClick: {
                        if (!heuristicZoom(clickX,clickY,2.5)) {
                            var zf = iflick.width / contentsSize.width
                            if (zf >= contentsScale)
                                zf = 2.0*contentsScale // zoom in (else zooming out)
                            doZoom(zf,clickX*zf,clickY*zf)
                        }
                    }

                    SequentialAnimation {
                        id: quickZoom

                      PropertyAction {
                          target: webView
                          property: "renderingEnabled"
                          value: false
                      }
                      ParallelAnimation {
                          NumberAnimation {
                              id: scaleAnim
                              target: webView
                              property: "contentsScale"
                              // the to property is set before calling
                              easing.type: Easing.Linear
                              duration: 200
                          }
                          NumberAnimation {
                              id: flickVX
                              target: iflick
                              property: "contentX"
                              easing.type: Easing.Linear
                              duration: 200
                              from: 0 // set before calling
                              to: 0 // set before calling
                          }
                          NumberAnimation {
                              id: flickVY
                              target: iflick
                              property: "contentY"
                              easing.type: Easing.Linear
                              duration: 200
                              from: 0 // set before calling
                              to: 0 // set before calling
                          }
                      }
                      // Have to set the contentXY, since the above 2
                      // size changes may have started a correction if
                      // contentsScale < 1.0.
                      PropertyAction {
                          id: finalX
                          target: iflick
                          property: "contentX"
                          value: 0 // set before calling
                      }
                      PropertyAction {
                          id: finalY
                          target: iflick
                          property: "contentY"
                          value: 0 // set before calling
                      }
                      PropertyAction {
                          target: webView
                          property: "renderingEnabled"
                          value: true
                      }
                     }
                      function doZoom(zoom,centerX,centerY)
                       {
                           if (centerX) {
                               var sc = zoom*contentsScale;
                               scaleAnim.to = sc;
                               flickVX.from = iflick.contentX
                               flickVX.to = Math.max(0,Math.min(centerX-iflick.width/2,webView.width*sc-iflick.width))
                               finalX.value = flickVX.to
                               flickVY.from = iflick.contentY
                               flickVY.to = Math.max(0,Math.min(centerY-iflick.height/2,webView.height*sc-iflick.height))
                               finalY.value = flickVY.to
                               quickZoom.start()
                           }
                       }


                 }
            }*/

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 30
            }

            HeadingText {
                text: qsTr("Add reply")
                color: UI.PAGE_HEADER_TITLE_COLOR
                font.bold: false
                visible: window.sppPostPublishStatus!=qsTr("Local Draft")
            }

            TextArea {
                id: addCommentField
                enabled: window.addCommentState != UI.ProgressState.Processing
                width: parent.width
//                font.pixelSize: window.appInputFontSize
                placeholderText: qsTr("Enter your reply here")
                height: Math.max (100, implicitHeight)
                visible: window.sppPostPublishStatus!=qsTr("Local Draft")
            }

            Row{
                anchors.left: parent.left
                anchors.right: parent.right
                visible: window.sppPostPublishStatus!=qsTr("Local Draft")

                BusyIndicator {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    platformStyle: BusyIndicatorStyle { size: "small" }
                    visible: window.addCommentState == UI.ProgressState.Processing
                    running: true
                }

                Button{
                    text: qsTr("Post")
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
                text: commentList.count!=0?qsTr("Comments(%1)").arg(commentList.count):qsTr("No comments yet")
                color: UI.PAGE_HEADER_TITLE_COLOR
                font.bold: false
                visible: window.sppPostPublishStatus!=qsTr("Local Draft")
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
                                font.pixelSize: titleText.font.pixelSize-2
                                elide: Text.ElideRight
                                color: UI.LISTDELEGATE_TEXT_COLOR
                                horizontalAlignment: Text.AlignRight
                            }

                            Text {
                                id: titleText
                                text: authorName
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
                                text: content
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
                        onPressAndHold: commentList.showMenu(index, localId, postId, commentStatus)
                    }
                }
            }

            ListView {
                id: commentList
                width: parent.width
                height: Math.max(contentHeight,5)
                cacheBuffer: height
                delegate: commentDelegate
                model: postCommentsModel
                clip: true
                enabled: !pageStack.busy
                spacing: 5
                interactive: false
                visible: window.sppPostPublishStatus!=qsTr("Local Draft")||count!=0
                property int selectedComment: -1
                onCountChanged: console.log("Helloooooo",window.sppPostPublishStatus,count,height)

                ContextMenu {
                    id: contextMenu
                    visualParent: postPage
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
    }


}
//![0]
