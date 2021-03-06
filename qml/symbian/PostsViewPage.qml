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
        ToolButton {
            id: button2
            flat: true
            iconSource: window.isIconsMetro?"qrc:/qml/images/reload.png":"qrc:/qml/images/symbian/symbian_reload.png"
            visible: !status.visible
            onClicked: window.refreshCurrentBlogPosts()
        }
        ToolButton {
            id: button3
            flat: true
            iconSource: window.isIconsMetro?"qrc:/qml/images/add.png":"qrc:/qml/images/symbian/symbian_add.png"
            onClicked: {
//                if(!marking){
                    newPostPage.resetPage()
                    var d = new Date()
                    var month = d.getMonth() + 1
                    newPostPage.postDate = d.getFullYear()+"/"+month+"/"+d.getDate();
                    newPostPage.postTime = d.getHours()+":"+d.getMinutes()+":"+d.getSeconds();
                    pageStack.push(newPostPage)
//                }
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
                    if(window.postModelStatus=="")
                        return qsTr("Posts")
                    else
                        return window.postModelStatus
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
                visible: window.postModelState==UI.ProgressState.Processing
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
                            visible: categories!="Uncategorized" &&  categories!=""
                            font.pixelSize: titleText.font.pixelSize-1
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR
                        }
                        Row {
                            anchors.topMargin: 10
                            spacing: 10
                            width: parent.width

                            Text {
                                text: dateCreated
                                visible: text!=""
                                width: parent.width
                                font.pixelSize: titleText.font.pixelSize-2
                                elide: Text.ElideRight
                                color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR1
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Image {
                                source: selected?"qrc:/qml/images/comments - white.png":window.isThemeInverted?UI.COMMENTSIMG:UI.COMMENTSIMG
                                anchors.right: parent.right
                                visible: publishStatus!="Local Draft"
                                Text {
                                    text: commentsCount>999?"999+":commentsCount
                                    color: selected?UI.PAGE_HEADER_TITLE_COLOR:"white"
                                    font.pixelSize: text.length>3?titleText.font.pixelSize-4:titleText.font.pixelSize-2
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: text.length>3?3:1
                                }
                            }
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            window.openPost(localId)
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
                        iconSource: {
                            if(selected) {
                                if(window.isIconsMetro)
                                    return "qrc:/qml/images/edit - white.png"
                                else
                                    return "qrc:/qml/images/symbian/symbian_edit_small.png"
                            } else {
                                if(window.isIconsMetro)
                                    return UI.EDITIMG
                                else{
                                    if(window.isThemeInverted)
                                        return "qrc:/qml/images/symbian/symbian_edit_small.png"
                                    else
                                        return "qrc:/qml/images/symbian/symbian_edit_small_black.png"
                                }
                            }
                        }
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
                            if(postPassword!=""){
                                newPostPage.postPwdText = postPassword
                            }
                            window.markCurrentPostCategories(localId)
                            pageStack.push(newPostPage)
                        }
                    }
                    ToolButton{
                        id: deleteButton
                        flat: true
                        width: 36; height: 36
                        iconSource: {
                            if(selected) {
                                if(window.isIconsMetro)
                                    return "qrc:/qml/images/delete - white.png"
                                else
                                    return "qrc:/qml/images/symbian/symbian_recycle bin_small.png"
                            } else {
                                if(window.isIconsMetro)
                                    return UI.DELETEIMG
                                else{
                                    if(window.isThemeInverted)
                                        return "qrc:/qml/images/symbian/symbian_recycle bin_small.png"
                                    else
                                        return "qrc:/qml/images/symbian/symbian_recycle bin_small_black.png"
                                }
                            }
                        }
                        onClicked: {
                            showDialog(qsTr("Do you really want to delete post \"<strong>%1</strong>\"?").arg(title), localId)
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

    ListView {
        id:postList
        anchors {
            top: header.bottom
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
