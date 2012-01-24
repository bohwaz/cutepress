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

    property string mediaSize: "Full"
    property string mediaAlignment: "None"
    property string htmlSnippet: ""

    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }


    tools: ToolBarLayout {
        ToolButton {
            id: button1
            flat: true
            iconSource: "qrc:/qml/images/back.png"
            onClicked: {
                window.replaceFile("AddMediaPage.qml")
            }
        }
        ToolButton {
            id: button2
            text: "Insert"
            onClicked: {
                var html = "\n<p><a href=\""+window.impUrlText+"\"><img class=\"align"+mediaAlignment.toLowerCase()+" size-"+mediaSize.toLowerCase()+" wp-image-6\" src=\""+window.impUrlText+"\" /></a></p>\n"
                if(window.impCaller == "page"){
                    newPagePage.insertMedia(html)
                    pageStack.replace(newPagePage)
                }else{
                    newPostPage.insertMedia(html)
                    pageStack.replace(newPostPage)
                }
            }
        }
    }

    Flickable {
        id: pageFlickable
        anchors.fill: parent
        anchors {
            fill: parent
            topMargin: UI.MARGIN_XLARGE
            leftMargin: UI.MARGIN_XLARGE
            rightMargin:  UI.MARGIN_XLARGE
        }
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
                    source: window.impThumbSource
                    visible: status == Image.Ready
                    width: window.inPortrait?parent.width:(parent.width-parent.spacing)/2
                    fillMode: Image.PreserveAspectFit
                }

                Column {
                    width: window.inPortrait?parent.width:(parent.width-parent.spacing)/2
                    spacing: 15

                    HeadingText {
                        text: "Title"
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: nameLabel
                        text: window.impNameText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                    }

                    HeadingText {
                        text: "Description"
                        visible: descLabel.text!=""
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: descLabel
                        text: window.impDescText
                        visible: text!=""
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                    }

                    HeadingText {
                        text: "Caption"
                        visible: captionLabel.text!=""
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: captionLabel
                        text: window.impCaptionText
                        visible: text!=""
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                        textFormat: Text.PlainText
                        elide: Text.ElideRight
                    }

                    HeadingText {
                        text: "File type"
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: typeLabel
                        text: window.impTypeText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                    }

                    HeadingText {
                        text: "Upload date"
                        color: UI.PAGE_HEADER_TITLE_COLOR
                    }

                    Text {
                        id: dateLabel
                        text: window.impDateText
                        font.pixelSize: window.appGeneralFontSize
                        color: UI.HEADINGTEXT_COLOR
                    }
                }
            }

            HeadingText {
                text: "File URL"
                color: UI.PAGE_HEADER_TITLE_COLOR
            }

            Text {
                id: urlLabel
                text: window.impUrlText
                font.pixelSize: window.appGeneralFontSize
                color: UI.HEADINGTEXT_COLOR
                textFormat: Text.PlainText
                elide: Text.ElideRight
                width: parent.width
            }

            HeadingText {
                text: "Alignment"
                color: UI.PAGE_HEADER_TITLE_COLOR
            }

            Button {
                id: setAlignmentInput
                font.pixelSize: window.appInputFontSize
                width: 0.8*parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                text: mediaAlignment
                onClicked: setAlignmentContextMenu.open()

                ContextMenu {
                    id: setAlignmentContextMenu
                    content:  MenuLayout {
                        MenuItem { text: "None"; onClicked: { mediaAlignment = text } }
                        MenuItem { text: "Left"; onClicked: { mediaAlignment = text } }
                        MenuItem { text: "Center"; onClicked: { mediaAlignment = text } }
                        MenuItem { text: "Right"; onClicked: { mediaAlignment = text } }
                    }
                }
            }

            HeadingText {
                text: "Size"
                color: UI.PAGE_HEADER_TITLE_COLOR
            }

            Button {
                id: setSizeInput
                font.pixelSize: window.appInputFontSize
                width: 0.8*parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                text: mediaSize
                onClicked: setSizeContextMenu.open()

                ContextMenu {
                    id: setSizeContextMenu
                    content:  MenuLayout {
                        MenuItem { text: "Thumbnail"; onClicked: { mediaSize = text } }
                        MenuItem { text: "Medium"; onClicked: { mediaSize = text } }
                        MenuItem { text: "Large"; onClicked: { mediaSize = text } }
                        MenuItem { text: "Full"; onClicked: { mediaSize = text } }
                    }
                }
            }
        }
    }
}
//![0]
