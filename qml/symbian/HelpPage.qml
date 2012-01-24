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
            iconSource: "qrc:/qml/images/back.png"
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
                text: "Help"
            }

            HeadingText{
                font.bold: false
                wrapMode: Text.Wrap
                onLinkActivated: Qt.openUrlExternally(link)
                text: "A video showing working of CutePress is available on <a style='color:orange' href='http://www.youtube.com/watch?v=Pb3LY93Az34'>YouTube</a>.<br>
                       Before adding a blog enable XML-RPC from the admin end, for this go to Dashboard->Settings->Writing->Remote Publishing and check XML-RPC.<br><br>
                       Blogs that are not hosted on WordPress.com does not support retrieving media items.<br><br>
                       <em>Symbols and icons related to the application are described below.</em>"
            }

            BigHeadingText{
                text: "Tool icons"
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
                    source: UI.BACKIMG
                }
                HeadingText{
                    text: "To move back from a particular view"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img1.verticalCenter
                }

                Image{
                    id: img2
                    source: UI.BLOGIMG
                }
                HeadingText{
                    text: "To view all added blogs"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter:img2.verticalCenter
                }

                Image{
                    id: img3
                    source: UI.CLOSEIMG
                }
                HeadingText{
                    text: "To remove an added blog"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter:img3.verticalCenter
                }

                Image{
                    id: img4
                    source: UI.EDITIMG
                }
                HeadingText{
                    text: "To edit a page/post in the corresponding view"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter:img4.verticalCenter
                }

                Image{
                    id: img5
                    source: UI.PUBLISHIMG
                }
                HeadingText{
                    text: "To publish posts/pages"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img5.verticalCenter
                }

                Image{
                    id: img6
                    source: UI.DELETEBIGIMG
                }
                HeadingText{
                    text: "To delete a page/post in the corresponding view"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img6.verticalCenter
                }

                Image{
                    id: img7
                    source: UI.RELOADIMG
                }
                HeadingText{
                    text: "To refresh a particular view"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img7.verticalCenter
                }

                Image{
                    id: img8
                    source: UI.SAVEIMG
                }
                HeadingText{
                    text: "To save posts/pages as draft or local draft"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: img8.verticalCenter
                }
            }


            BigHeadingText{
                text: "Status in pages/posts view"
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
                    text: "Publicly published page/post"
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
                    text: "Private page/post"
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
                    text: "Page/post scheduled for publishing in future"
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
                    text: "Page/Post pending for review"
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
                    text: "Local or server draft page/post"
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
                    text: "Trashed page/post"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: prect6.verticalCenter
                }
            }


            BigHeadingText{
                text: "Status in comments view"
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
                    text: "Approved comment"
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
                    text: "Unapproved comment"
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
                    text: "Deleted or spammed comment"
                    wrapMode: Text.WordWrap
                    font.bold: false
                    width: parent.width-45
                    anchors.verticalCenter: crect3.verticalCenter
                }
            }
        }
    }
}
