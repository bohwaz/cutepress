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
            onClicked: window.refreshCurrentBlogPages()
        }
        ToolButton {
            id: button3
            flat: true
            iconSource: window.isIconsMetro?"qrc:/qml/images/add.png":"qrc:/qml/images/symbian/symbian_add.png"
            onClicked: {
                newPagePage.resetPage()
                var d = new Date()
                var month = d.getMonth() + 1
                newPagePage.pageDate = d.getFullYear()+"/"+month+"/"+d.getDate();
                newPagePage.pageTime = d.getHours()+":"+d.getMinutes()+":"+d.getSeconds();
                pageStack.push(newPagePage)
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
                    if(window.pageModelStatus=="")
                        return qsTr("Pages")
                    else
                        return window.pageModelStatus
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
                visible: window.pageModelState==UI.ProgressState.Processing
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
        id: pageDelegate
        Item {
            property bool selected: mouseArea.pressed//myCurrentItem==index

            id: wrapper
            width: wrapper.ListView.view.width
            height: Math.max(contentRow.childrenRect.height, 82)

            Rectangle { id: bgRect; anchors.fill: parent; color: selected?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}

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
                    width: parent.width - 2*parent.spacing - buttonColumn.width - 10
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
                            text:{
                                var txt = "<strong>" + title + "</strong>"
                                if(pagePassword!="")
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
                            text: dateCreated;
                            width: parent.width
                            font.pixelSize: titleText.font.pixelSize-2
                            elide: Text.ElideRight
                            color: selected?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TEXT_COLOR1
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
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
                                    return "qrc:/qml/images/symbian/symbian_edit.png"
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
                            newPagePage.resetPage()
                            newPagePage.isEditingPage = true
                            newPagePage.pageLocalId = localId
                            newPagePage.pagePublishStatus = publishStatus
                            newPagePage.pageTitleText = title
                            newPagePage.pageContentText = description
                            var d =  new Date(dateCreated)
                            newPagePage.pageDate = pageDate
                            newPagePage.pageTime = pageTime
                            newPagePage.pagePwdText = pagePassword
                            pageStack.push(newPagePage)
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
                        onClicked: showDialog(qsTr("Doy you really want to delete post \"<strong>%1</strong>\"?").arg(title), localId)//deletePage(localId)
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
        id:pageList
        anchors {
            top: header.bottom
            left: parent.left
            leftMargin: UI.MARGIN_XLARGE
            right: parent.right
            rightMargin: UI.MARGIN_XLARGE
            bottom: parent.bottom
        }
        cacheBuffer: 2 * height
        delegate: pageDelegate
        model: pagesModel
        clip: true
        enabled: !pageStack.busy
        spacing: 5
    }

    QueryDialog {
        id: query
        property string myPageId: ""
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        titleText: qsTr("Delete page")
        onAccepted: deletePage(myPageId)
    }

    function showDialog(msg, id){
        query.message = msg
        query.myPageId = id
        query.open()
    }
}
//![0]
