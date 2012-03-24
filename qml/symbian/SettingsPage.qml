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
    id: page
    anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
    tools: ToolBarLayout {
        ToolButton {
            iconSource: window.isIconsMetro?"qrc:/qml/images/back.png":"qrc:/qml/images/symbian/symbian_back.png"
            onClicked: {
                window.saveSettings(window.isThemeInverted?"dark":"light", window.isIconsMetro?"metro":"default")
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
                text: qsTr("Settings")
                color: window.isThemeInverted?UI.PAGE_HEADER_TITLE_COLOR:UI.PAGE_HEADER_TITLE_COLOR
                wrapMode: Text.Wrap
            }

            ListSectionDelegate {
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                margins: 0
                sectionName: qsTr("Custom media folders")
                color: window.isThemeInverted?UI.LISTSECTION_TITLE_COLOR:UI.LISTSECTION_TITLE_COLOR;
            }

            Component {
                 id: upDirDelegate
                 Item {
                     width: parent.width
                     height: delButton.height
                     Rectangle { anchors.fill: parent; color: window.isThemeInverted?UI.LISTDELEGATE_BG_COLOR_PRESSED:UI.LISTDELEGATE_BG_COLOR_PRESSED; }
                     Text {
                         text: dirPath
                         anchors.leftMargin: 5
                         anchors.left: parent.left
                         anchors.right: delButton.left
                         anchors.verticalCenter: delButton.verticalCenter
                         font.pixelSize: window.appGeneralFontSize
                         elide: Text.ElideLeft
                         color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
                     }
                     ToolButton {
                         id: delButton
                         height: 36
                         flat: true
                         iconSource: window.isIconsMetro?"qrc:/qml/images/close.png":window.isThemeInverted?"qrc:/qml/images/symbian/symbian_close.png":"qrc:/qml/images/symbian/symbian_close_black.png"
                         anchors.rightMargin: 5
                         anchors.right: parent.right
                         onClicked: removeExistingDir(index)
                     }
                 }
             }

            //Add new image folder starts
            HeadingText {
                text: qsTr("Add new")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            Item {
                id: abc
                width: parent.width
                height: Math.max(addButton.height, newDir.height)+10
                TextField {
                    id: newDir
                    text: window.newSelectedDir
                    anchors.leftMargin: 5
                    anchors.left: parent.left
                    anchors.rightMargin: 5
                    anchors.right: dialogButton.left
                    anchors.verticalCenter: addButton.verticalCenter
                }
                ToolButton {
                    id: dialogButton
                    flat: true
                    anchors.rightMargin: 5
                    iconSource: window.isIconsMetro?"qrc:/qml/images/dir.png":window.isThemeInverted?"qrc:/qml/images/symbian/symbian_dir.png":"qrc:/qml/images/symbian/symbian_dir_black.png"
                    anchors.right: addButton.left
                    onClicked: getDirectory()
                }
                ToolButton {
                    id: addButton
                    flat: true
                    anchors.rightMargin: 5
                    iconSource: window.isIconsMetro?"qrc:/qml/images/add.png":window.isThemeInverted?"qrc:/qml/images/symbian/symbian_add.png":"qrc:/qml/images/symbian/symbian_add_black.png"
                    anchors.right: parent.right
                    onClicked: {
                        if(newDir.text!="") {
                            addNewDir(newDir.text);
                            newDir.text = ""
                            window.newSelectedDir = ""
                        }
                    }
                }
            }// end of add new folder

            //Current image folders list starts
            HeadingText {
                text: qsTr("Current custom media folders")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            ListView {
                id: mediaDirsList
                anchors {
                    left: parent.left
                    right: parent.right
                }
                width: parent.width
                interactive: false
                height: Math.max(5, contentHeight)
                clip: true
                model: mediaDirModel
                delegate: upDirDelegate
                spacing: 5
                focus: true
                visible: count!=0
            }

            Text {
                text: qsTr("No custom media folders found")
                width: 0.75*parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: window.isThemeInverted?UI.PROGRESSSTATUS_LABEL_COLOR:UI.PROGRESSSTATUS_LABEL_COLOR
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: window.appGeneralFontSize+5
                visible: mediaDirsList.count === 0
            }

            ListSectionDelegate {
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                margins: 0
                sectionName: qsTr("Appearance")
                color: window.isThemeInverted?UI.LISTSECTION_TITLE_COLOR:UI.LISTSECTION_TITLE_COLOR;
            }

            HeadingText {
                text: qsTr("Theme")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            ButtonRow {
                id: themeButtonRow
                width: 0.9*parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: lightButton
                    text: qsTr("Light")
                    font.pixelSize: window.appGeneralFontSize
                    onClicked: {
                        if(window.isThemeInverted) {
                            window.isThemeInverted = !window.isThemeInverted
                        }
                    }
                }
                Button {
                    id: darkButton
                    text: qsTr("Dark")
                    font.pixelSize: window.appGeneralFontSize
                    onClicked: {
                        if(!window.isThemeInverted) {
                            window.isThemeInverted = !window.isThemeInverted
                        }
                    }
                }
            }

            HeadingText {
                text: qsTr("Icons")
                color: window.isThemeInverted?UI.HEADINGTEXT_COLOR:UI.HEADINGTEXT_COLOR
            }

            ButtonRow {
                id: iconTypeButtonRow
                width: 0.9*parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: metroButton
                    text: qsTr("Metro")
                    font.pixelSize: window.appGeneralFontSize
                    onClicked: {
                        window.isIconsMetro = true
                    }
                }
                Button {
                    id: defaultButton
                    text: qsTr("Default")
                    font.pixelSize: window.appGeneralFontSize
                    onClicked: {
                        window.isIconsMetro = false
                    }
                }
            }

            ListSectionDelegate {
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                margins: 0
                sectionName: qsTr("Thumbnail cache")
                color: window.isThemeInverted?UI.LISTSECTION_TITLE_COLOR:UI.LISTSECTION_TITLE_COLOR;
            }

            Button {
                text: qsTr("Delete")
                width: 0.75*parent.width
                font.pixelSize: window.appGeneralFontSize
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
//                    banner.show()
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
        if(window.isIconsMetro)
            iconTypeButtonRow.checkedButton = metroButton
        else
            iconTypeButtonRow.checkedButton = defaultButton
    }

//    InfoBanner {
//         id: banner
//         text: qsTr("Deleting thumbnails for local media items")
//     }
}
