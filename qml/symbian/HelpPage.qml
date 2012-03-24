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
    id: helpPage
    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
    tools: ToolBarLayout {
        ToolButton {
            flat: true
            iconSource: window.isIconsMetro?"qrc:/qml/images/back.png":"qrc:/qml/images/symbian/symbian_back.png"
            onClicked: pageStack.pop();
        }
    }

    Flickable {
        id: flickable
        anchors {
            fill: parent
            topMargin: UI.MARGIN_XLARGE
            leftMargin: UI.MARGIN_XLARGE
            rightMargin:  UI.MARGIN_XLARGE
        }
        contentWidth: width
        contentHeight: content.height
        flickableDirection: Flickable.VerticalFlick
        interactive: contentHeight>height

        Column {
            id: content
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            spacing: UI.DETAILS_SPACING

            BigHeadingText{
                text: qsTr("Help")
            }

            HeadingText{
                font.bold: false
                wrapMode: Text.Wrap
                onLinkActivated: Qt.openUrlExternally(link)
                text: qsTr("A video showing working of CutePress is available on <a style='color:orange' href='http://www.youtube.com/watch?v=Pb3LY93Az34'>YouTube</a>")+
                      "<br>"+
                      qsTr("Before adding a blog enable XML-RPC from the admin end, for this go to Dashboard->Settings->Writing->Remote Publishing and check XML-RPC.")+
                      "<br><br>"+
                      qsTr("Blogs that are not hosted on WordPress.com does not support retrieving media items.")+
                      "<br><br>"+
                      qsTr("<em>Symbols and icons related to the application are described below.</em>")
            }

            BigHeadingText{
                text: qsTr("Tool icons")
                font.pixelSize: window.appGeneralFontSize
            }

            Grid{
                columns: 2
                spacing: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 10

                Image{
                    id: img1
                    source: window.isIconsMetro?UI.BACKIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_back.png":"qrc:/qml/images/symbian/symbian_back_black.png"
                }
                HeadingText{
                    text: qsTr("To move back from a particular view")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img1.verticalCenter
                }

//                Image{
//                    id: img2
//                    source: window.isIconsMetro?UI.BLOGIMG
//                }
//                HeadingText{
//                    text: qsTr("To view all added blogs")
//                    wrapMode: Text.WordWrap
//                    font.bold: false
//                    width: parent.width-45
//                    anchors.verticalCenter:img2.verticalCenter
//                }

                Image{
                    id: img3
                    source: window.isIconsMetro?UI.CLOSEIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_close.png":"qrc:/qml/images/symbian/symbian_close_black.png"
                }
                HeadingText{
                    text: qsTr("To remove an added blog")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter:img3.verticalCenter
                }

                Image{
                    id: img4
                    source: window.isIconsMetro?UI.EDITIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_edit.png":"qrc:/qml/images/symbian/symbian_edit_black.png"
                }
                HeadingText{
                    text: qsTr("To edit a page/post in the corresponding view")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter:img4.verticalCenter
                }

                Image{
                    id: img5
                    source: window.isIconsMetro?UI.PUBLISHIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_publish.png":"qrc:/qml/images/symbian/symbian_publish_black.png"
                }
                HeadingText{
                    text: qsTr("To publish posts/pages")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img5.verticalCenter
                }

                Image{
                    id: img6
                    source: window.isIconsMetro?UI.DELETEBIGIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_recycle bin.png":"qrc:/qml/images/symbian/symbian_recycle bin_black.png"
                }
                HeadingText{
                    text: qsTr("To delete a page/post in the corresponding view")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img6.verticalCenter
                }

                Image{
                    id: img7
                    source: window.isIconsMetro?UI.RELOADIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_reload.png":"qrc:/qml/images/symbian/symbian_reload_black.png"
                }
                HeadingText{
                    text: qsTr("To refresh a particular view")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img7.verticalCenter
                }

                Image{
                    id: img8
                    source: window.isIconsMetro?UI.SAVEIMG:window.isThemeInverted?"qrc:/qml/images/symbian/symbian_save.png":"qrc:/qml/images/symbian/symbian_save_black.png"
                }
                HeadingText{
                    text: qsTr("To save posts/pages as draft or local draft")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img8.verticalCenter
                }
            }


            BigHeadingText{
                text: qsTr("Status in pages/posts view")
                font.pixelSize: window.appGeneralFontSize
            }

            Grid{
                columns: 2
                spacing: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 10

                Rectangle {
                    id: prect1
                    color: "green"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Publicly published page/post")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    anchors.verticalCenter: prect1.verticalCenter
                }

                Rectangle {
                    id: prect2
                    color: "lightgreen"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Private page/post")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: prect2.verticalCenter
                }

                Rectangle {
                    id: prect3
                    color: "#5674b9"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Page/post scheduled for publishing in future")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: prect3.verticalCenter
                }

                Rectangle {
                    id: prect4
                    color: "yellow"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Page/Post pending for review")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: prect4.verticalCenter
                }

                Rectangle {
                    id: prect5
                    color: "orange"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Local or server draft page/post")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: prect5.verticalCenter
                }

                Rectangle {
                    id: prect6
                    color: "red"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Trashed page/post")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: prect6.verticalCenter
                }
            }


            BigHeadingText{
                text: qsTr("Status in comments view")
                font.pixelSize: window.appGeneralFontSize
            }

            Grid{
                columns: 2
                spacing: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 10

                Rectangle {
                    id: crect1
                    color: "green"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Approved comment")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: crect1.verticalCenter
                }

                Rectangle {
                    id: crect2
                    color: "orange"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Unapproved comment")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: crect2.verticalCenter
                }

                Rectangle {
                    id: crect3
                    color: "red"
                    width: 25
                    height: 10
                }
                HeadingText{
                    text: qsTr("Deleted or spammed comment")
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: crect3.verticalCenter
                }
            }
        }
    }
}
