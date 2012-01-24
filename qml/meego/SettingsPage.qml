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
    id: page

    tools: ToolBarLayout {
        ToolIcon {
            iconSource: window.isThemeInverted?UI.BACKIMG:UI.BACKIMG
            onClicked: {
                window.saveSettings(window.isThemeInverted?"dark":"light")
                pageStack.pop();
            }
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
        contentHeight: content.childrenRect.height + UI.MARGIN_XLARGE * 2
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

            BigHeadingText {
                id: heading
                text: "Settings"
                color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
                wrapMode: Text.Wrap
            }

            ListSectionDelegate {
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                margins: 0
                sectionName: "Theme"
                color: window.isThemeInverted?UI.LISTSECTION_TITLE_COLOR:UI.LISTSECTION_TITLE_COLOR;
            }

            ButtonRow {
                id: themeButtonRow
                width: 0.9*parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: lightButton
                    text: "Light"
                    font.pixelSize: window.appGeneralFontSize
                    onClicked: {
                        if(window.isThemeInverted) {
                            window.isThemeInverted = !window.isThemeInverted
                        }
                    }
                }
                Button {
                    id: darkButton
                    text: "Dark"
                    font.pixelSize: window.appGeneralFontSize
                    onClicked: {
                        if(!window.isThemeInverted) {
                            window.isThemeInverted = !window.isThemeInverted
                        }
                    }
                }
            }

            ListSectionDelegate {
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                margins: 0
                sectionName: "Thumbnail cache"
                color: window.isThemeInverted?UI.LISTSECTION_TITLE_COLOR:UI.LISTSECTION_TITLE_COLOR;
            }

            Button {
                text: "Delete"
                width: 0.75*parent.width
                font.pixelSize: window.appGeneralFontSize
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    banner.show()
                    window.refreshThumbnailCache()
                }
            }
        }
    }
    Component.onCompleted: {
        if(window.isThemeInverted)
            themeButtonRow.checkedButton = darkButton
        else
            themeButtonRow.checkedButton = lightButton
    }

    InfoBanner {
         id: banner
         text: qsTr("Deleting thumbnails for local media items")
     }
}
