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
    id: aboutPage

    tools: ToolBarLayout {
        ToolIcon {
            iconSource: UI.BACKIMG
            onClicked: pageStack.pop();
        }
    }

    Flickable {
        id: flickable        
        anchors {
            fill: parent
            topMargin: UI.MARGIN_XLARGE
            leftMargin: UI.MARGIN_XLARGE
            rightMargin: UI.MARGIN_XLARGE
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

            Row {
                anchors.margins: UI.MARGIN_XLARGE
                width: parent.width
                spacing: window.inPortrait?10:30

                Image {
                    id: logoImg
                    source: 'qrc:/qml/images/CutePress-logo.png';
                    width: Math.min(parent.width - headDetail.width - parent.spacing, sourceSize.width)
                    height: width
                    smooth: true
                    anchors.verticalCenter: headDetail.verticalCenter
                }

                Text {
                    id: headDetail
                    property string appVer: "1.2.0"
                    font.pixelSize: window.appGeneralFontSize+2;
                    text: qsTr("<strong>CutePress version %1</strong>").arg(appVer)+"<br\>"+
                          qsTr("Developed by Abhishek Kumar.")+"<br\>"+
                          qsTr("Copyright© 2012 Abhishek Kumar.")
                    color: UI.HEADINGTEXT_COLOR
                    wrapMode: TextEdit.WordWrap
                    anchors.verticalCenter: parent.verticalCenter
                }

            }

            Text {
                id: aboutText
                font.pixelSize: window.appGeneralFontSize;
                text: qsTr("Please send your feedback to <a style='color:orange' href='mailto:abhishek.mrt22@gmail.com'>abhishek.mrt22@gmail.com</a>. All comments and ideas are welcome to further improve CutePress.")+"<br/><br/>"+
                      qsTr("<i>Overview</i>")+"<br\>"+qsTr("CutePress allows user manage pages, posts, comments and media items on WordPress blogs. It needs an active GPRS connection or Wi-fi (on Wif-fi enabled devices) to access Internet.")+"<br/><br/>"+
                      qsTr("It supports WordPress 2.7 or higher(WordPress.com or self-hosted).")+"<br/><br/>"+
                      qsTr("Turkish translation by <a style='color:orange' href='http://www.meegoturkiye.org/'>MeeGo Turkey</a>.")+"<br/><br/>"+
                      qsTr("For more info visit <a style='color:orange' href='http://theshowstopper.in/cutepress'>http://theshowstopper.in/cutepress</a>")+"<br><br>"+
                      qsTr("If you really like this app and wants to donate, send your donations through PayPal on this id: <strong>abhishek.mrt22@gmail.com</strong>");
                width: parent.width
                color: UI.HEADINGTEXT_COLOR
                wrapMode: TextEdit.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
