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
    id: smediaPage

    tools: ToolBarLayout {
        ToolIcon {
            id: button1
            iconSource: UI.BACKIMG
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
                    visible: (status == Image.Ready)
                    width: window.inPortrait?parent.width:0.5*(parent.width-parent.spacing)
                    fillMode: Image.PreserveAspectFit
                }

                Column {
                    width: window.inPortrait?parent.width:0.5*(parent.width-parent.spacing)
                    spacing: 15

                    HeadingText {
                        text: "Title"
                        color: UI.FONT_COLOR_BLUE
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
                        text: "Description"
                        visible: descLabel.text!=""
                        color: UI.FONT_COLOR_BLUE
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
                        text: "Caption"
                        visible: captionLabel.text!=""
                        color: UI.FONT_COLOR_BLUE
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
                        text: "File type"
                        color: UI.FONT_COLOR_BLUE
                    }

                    Text {
                        id: typeLabel
                        text: window.smpTypeText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                    }

                    HeadingText {
                        text: "Upload date"
                        color: UI.FONT_COLOR_BLUE
                    }

                    Text {
                        id: dateLabel
                        text: window.smpDateText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                    }

                    HeadingText {
                        text: "File URL"
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
