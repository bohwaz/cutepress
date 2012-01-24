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
            iconSource: "qrc:/qml/images/back.png"
            enabled: window.addNewBlogStatus != UI.ProgressState.Processing
            onClicked: {
                pageStack.pop()
            }
        }
        ToolButton {
            id: button2
            flat: true
            iconSource: "qrc:/qml/images/forward.png"
            enabled: window.addNewBlogStatus != UI.ProgressState.Processing
            onClicked: {
                window.addNewBlogStatus = UI.ProgressState.Processing
                if(window.nbpIsEditingBlog){
                    window.addNewBlogMsg = "Editing\.\.\."
                    editBlog(window.nbpBlogId,blogUrl.text,blogUsr.text,blogPwd.text,resizeCheck.checked)
                }
                else{
                    window.addNewBlogMsg = "Connecting\.\.\."
                    addNewBlog(blogUrl.text,blogUsr.text,blogPwd.text,resizeCheck.checked)
                }
            }
        }
    }

    Flickable {
        id: pageFlickable
        anchors {
            fill: parent
            topMargin: UI.MARGIN_XLARGE
            leftMargin: UI.MARGIN_XLARGE
            rightMargin:  UI.MARGIN_XLARGE
        }
        contentHeight: content.height
        contentWidth: width
        flickableDirection: Flickable.VerticalFlick
        interactive: contentHeight>height

        Column {
            id: content
            visible: !pageStack.busy
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            spacing: 25

            Row {
                visible: window.addNewBlogStatus!=UI.ProgressState.None
                width: parent.width
                height: 32
                spacing: 12

                HeadingText {
                    text: window.addNewBlogMsg
                    wrapMode: Text.Wrap
                    anchors.verticalCenter: parent.verticalCenter
                    width: status.visible?parent.width-parent.spacing-status.width:parent.width
                }

                BusyIndicator {
                    id: status
                    anchors.right: parent.right
                    visible: window.addNewBlogStatus == UI.ProgressState.Processing
                    running: true
                    width: 32; height: 32
                }
            }

            Image {
                source: "qrc:/qml/images/wp-logo.png"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            BigHeadingText {
                text: window.nbpIsEditingBlog?"":"Start by entering blog info"
            }

            Row {
                width: parent.width
                spacing: 10
                HeadingText {
                    id: urlLabel
                    width: 35
                    text: "Url"
                    anchors.verticalCenter: parent.verticalCenter
                }
                TextField {
                    id: blogUrl
                    text: window.nbpUrlText
                    width: parent.width - urlLabel.width - parent.spacing
                    font.pixelSize: window.appInputFontSize
                    placeholderText: "Enter blog url here"
                    readOnly: window.nbpIsEditingBlog
                }
            }

            Row {
                width: parent.width
                spacing: 10
                HeadingText {
                    id: usernameLabel
                    width: 90
                    text: "Username"
                    anchors.verticalCenter: parent.verticalCenter
                }
                TextField {
                    id: blogUsr
                    width: parent.width - usernameLabel.width - parent.spacing
                    font.pixelSize: window.appInputFontSize
                    placeholderText: "Enter blog username here"
                    text: window.nbpUsernameText
                }
            }

            Row {
                width: parent.width
                spacing: 10
                HeadingText {
                    id: pwdLabel
                    width: 90
                    text: "Password"
                    anchors.verticalCenter: parent.verticalCenter
                }
                TextField {
                    id: blogPwd
                    width: parent.width - pwdLabel.width - parent.spacing
                    font.pixelSize: window.appInputFontSize
                    placeholderText: "Enter password here"
                    echoMode: TextInput.Password
                    text: window.nbpPasswordText
                }
            }
            Row {
                width: parent.width
                spacing: 10
                visible: false
                Column {
                    width: parent.width - resizeCheck.width - parent.spacing
                    spacing: 5

                    HeadingText {
                        text: "Resize images"
                        width: parent.width
                    }
                    SmallHeadingText {
                        width: parent.width
                        text: "Resizing will result in faster publishing but smaller photos"
                        wrapMode: Text.Wrap
                    }
                }
                CheckBox {
                    id: resizeCheck
                    checked: true
                }
            }
        }
    }
}
//![0]
