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
    id: pagesPage

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
            onClicked: window.refreshCurrentBlogPosts()
        }
        ToolIcon {
            id: button3
            iconSource: UI.ADDIMG
            onClicked: {
                if(!marking){
                    newPostPage.resetPage()
                    var d = new Date()
                    var month = d.getMonth() + 1
                    newPostPage.postDate = d.getFullYear()+"/"+month+"/"+d.getDate();
                    newPostPage.postTime = d.getHours()+":"+d.getMinutes()+":"+d.getSeconds();
                    pageStack.push(newPostPage)
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
                    if(window.postModelStatus=="")
                        return qsTr("Posts")
                    else
                        return window.postModelStatus
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
                visible: window.postModelState==UI.ProgressState.Processing
            }

            /*ToolButton {
                id: refreshButton
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                width: 32
                height: width
                flat: true
                window.isIconsMetro?"qrc:/qml/images/reload.png":"qrc:/qml/images/symbian/symbian_reload.png"
                visible: !status.visible
                enabled: !pageStack.busy
                onClicked: window.refreshCurrentBlogPosts()
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
        id: postDelegate
        Item {
            property bool selected: mouseArea.pressed

            id: wrapper
            width: wrapper.ListView.view.width
            anchors.margins: 0
            height: Math.max(contentRow.childrenRect.height, 82)

            Rectangle { id: bgRect; anchors.fill: parent; color: selected?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

            Row {
                id: contentRow
                anchors.margins: 0
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: UI.LISTDELEGATE_SPACING
                Item {
                    height: textColumn.childrenRect.height+textColumn.anchors.margins
                    width: parent.width - 2*parent.spacing - buttonColumn.width - 10
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
                            text:{
                                var txt = qsTr("<strong>%1</strong>").arg(title)
                                if(postPassword!="")
                                    txt = qsTr("[Protected] <strong>%1</strong>").arg(title)
                                return txt
                            }
                            color: selected?UI.LISTDELEGATE_TITLE_COLOR_MARKED:UI.LISTDELEGATE_TITLE_COLOR
                            width: parent.width
                            textFormat: Text.RichText
                            elide: Text.ElideRight
                            wrapMode: Text.Wrap
                            font.pixelSize: window.appGeneralFontSize
                        }

                        Text {
                            text: qsTr("by %1").arg(wpAuthorDisplayName);
                            width: parent.width
                            visible: wpAuthorDisplayName!=""
                            font.pixelSize: titleText.font.pixelSize-1
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR1
                        }

                        Text {
                            text: publishStatus;
                            width: parent.width
                            visible: (publishStatus == qsTr("Draft") || publishStatus == qsTr("Local Draft") || publishStatus == qsTr("Busy")||
                                       publishStatus == qsTr("Trashed") || publishStatus == qsTr("Orphaned"))
                            font.pixelSize: titleText.font.pixelSize-1
                            elide: Text.ElideRight
                            color: {
                                if(publishStatus == qsTr("Draft")||publishStatus == qsTr("Local Draft") || publishStatus == qsTr("Busy"))
                                    return "orange";
                                else if(publishStatus == qsTr("Trashed")||publishStatus == qsTr("Orphaned"))
                                    return "red";
                            }
                        }

                        Text {
                            text: qsTr("Categorized in %1").arg(categories);
                            width: parent.width
                            visible: categories!="Uncategorized" && categories!=""
                            font.pixelSize: titleText.font.pixelSize-1
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR:UI.LISTDELEGATE_TEXT_COLOR
                        }
                        Row {

                            anchors.topMargin: 10
                            spacing: 10
                            width: parent.width

                            Text {
                                text: dateCreated;
                                visible: text!=""
                                width: parent.width
                                font.pixelSize: titleText.font.pixelSize-2
                                elide: Text.ElideRight
                                color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:window.isThemeInverted?UI.LISTDELEGATE_TEXT_COLOR1:UI.LISTDELEGATE_TEXT_COLOR1
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Image {
                                source: selected?"qrc:/qml/images/comments - meego - white.png":UI.MEEGOCOMMENTSIMG
                                anchors.right: parent.right
                                visible: publishStatus!="Local Draft"
                                Text {
                                    text: commentsCount>999?"999+":commentsCount
                                    color: selected?UI.PAGE_HEADER_TITLE_COLOR:"white"
                                    font.pixelSize: text.length>3?titleText.font.pixelSize-4:titleText.font.pixelSize-2
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: text.length>3?2:1
                                }
                            }
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            window.openPost(localId)
                            //pageStack.push(blogOptionsPage)
                        }
//                        onPressed: myCurrentItem = index
//                        onReleased: myCurrentItem = -1
//                        onFocusChanged: {
//                            if(!focus)
//                                myCurrentItem = -1
//                        }
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
                            newPostPage.resetPage()
                            newPostPage.isEditingPost = true
                            newPostPage.postLocalId = localId
                            newPostPage.postPublishStatus = publishStatus
                            newPostPage.postTitleText = title
                            newPostPage.postPageText = description
                            newPostPage.postCatText = categories
                            var d =  new Date(dateCreated)
                            newPostPage.postDate = postDate
                            newPostPage.postTime = postTime
                            newPostPage.postPwdText = postPassword
                            window.markCurrentPostCategories(localId)
                            pageStack.push(newPostPage)
                        }
                    }
                    ToolIcon {
                        id: deleteButton
                        height: 45
                        iconSource: selected?"qrc:/qml/images/delete - white.png":window.isThemeInverted?UI.DELETEIMG:UI.DELETEIMG
                        onClicked: {
                            showDialog(qsTr("Doy you really want to delete post \"<strong>%1</strong>\"?").arg(title), localId)
                            //deletePost(localId)
                        }
                    }
                }
                Rectangle {
                    width: 5
                    height: Math.max(textColumn.childrenRect.height, buttonColumn.childrenRect.height-5)
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 5
                    color: {
                        if(publishStatus == qsTr("Published"))
                            return "green";
                        else if(publishStatus == qsTr("Private"))
                            return "lightgreen";
                        else if(publishStatus == qsTr("Scheduled in future"))
                            return "#5674b9";
                        else if(publishStatus == qsTr("Pending review"))
                            return "yellow";
                        else if(publishStatus == qsTr("Draft")||publishStatus == qsTr("Local Draft") || publishStatus == qsTr("Busy"))
                            return "orange";
                        else if(publishStatus == qsTr("Trashed")||publishStatus == qsTr("Orphaned"))
                            return "red";
                    }
                    anchors.right: parent.right
                }
            }
        }
    }

    /*Component {
        id: footerItem
        HeadingText{
            text: qsTr("Load more\.\.\.")
        }
    }*/

    ListView {
        id:postList
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
        delegate: postDelegate
        model: postsModel
        clip: true
        enabled: !pageStack.busy
        spacing: 5
        interactive: contentHeight>height
    }

    QueryDialog {
        id: query
        property string myPostId: ""
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        titleText: qsTr("Delete post")
        onAccepted: deletePost(myPostId)
    }

    function showDialog(msg, id){
        query.message = msg
        query.myPostId = id
        query.open()
    }
}
//![0]
