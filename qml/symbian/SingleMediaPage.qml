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
            iconSource: window.isIconsMetro?"qrc:/qml/images/back.png":"qrc:/qml/images/symbian/symbian_back.png"
            onClicked: {
                pageStack.pop()
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
            spacing: 15

            Grid {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 15
                columns: window.inPortrait?1:2

                Image {
                    id: thumb
                    source: window.smpUrlText
                    visible: status == Image.Ready
                    width: window.inPortrait?parent.width:(parent.width-parent.spacing)/2
                    fillMode: Image.PreserveAspectFit
                }

                Column {
                    width: window.inPortrait?parent.width:(parent.width-parent.spacing)/2
                    spacing: 15

                    HeadingText {
                        text: qsTr("Title")
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: nameLabel
                        text: window.smpNameText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                    }

                    HeadingText {
                        text: qsTr("Description")
                        visible: descLabel.text!=""
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: descLabel
                        text: window.smpDescText
                        visible: text!=""
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                    }

                    HeadingText {
                        text: qsTr("Caption")
                        visible: captionLabel.text!=""
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: captionLabel
                        text: window.smpCaptionText
                        visible: text!=""
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                    }

                    HeadingText {
                        text: qsTr("File type")
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: typeLabel
                        text: window.smpTypeText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                    }

                    HeadingText {
                        text: qsTr("Upload date")
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: dateLabel
                        text: window.smpDateText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                    }

                    HeadingText {
                        text: qsTr("File URL")
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: urlLabel
                        text: window.smpUrlText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        wrapMode: Text.Wrap
                        width: parent.width
                    }
                }
            }
        }
    }
}
//![0]
