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
    id: categoryPage

    tools: ToolBarLayout {
        ToolIcon {
            id: button1
            iconSource: UI.BACKIMG
            onClicked: {
                window.setCategoriesToPost("")
                pageStack.pop()
            }
        }
        ToolIcon {
            id: button2
            iconSource: UI.ADDIMG
            onClicked: {
                addCatDialog.catTitleText = ""
                addCatDialog.open()
            }
        }
    }

    Item {
        id: header
        height: headerRow.height+UI.MARGIN_XLARGE
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
                bottomMargin: UI.MARGIN_XLARGE
            }

            BigHeadingText {
                id: heading
                anchors.verticalCenter: parent.verticalCenter
                text: window.addCategoryState==2?window.addCategoryStatus:qsTr("Select Category")
                width: parent.width-parent.spacing-refreshButton.width
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                color: UI.PAGE_HEADER_TITLE_COLOR
            }

            BusyIndicator {
                id: status
                visible: window.addCategoryState==1
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                platformStyle: BusyIndicatorStyle { size: "small" }
                running: true
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
        id: categoryDelegate
        Item {
            id: wrapper

            width: wrapper.ListView.view.width
            height: Math.max(titleText.height, 48)

            Rectangle { id: bgRect; anchors.fill: parent; color: isMarked?UI.LISTDELEGATE_WRAPPER_COLOR_MARKED:UI.LISTDELEGATE_WRAPPER_COLOR; opacity: 0.8}
            Text {
                id: titleText
                text: catName
                color: isMarked?UI.LISTDELEGATE_TEXT_COLOR_MARKED:UI.LISTDELEGATE_TITLE_COLOR
                width: parent.width
                textFormat: Text.RichText
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                font.pixelSize: window.appGeneralFontSize
                font.bold: isMarked
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.rightMargin: 5
            }

            MouseArea{
                anchors.fill: parent
                enabled: !pageStack.busy
                onClicked: window.markCategory(catName)
            }
        }
    }

    ListView {
        id:categoryList
        anchors {
            top: header.bottom
            topMargin: UI.MARGIN_XLARGE
            left: parent.left
            leftMargin: UI.MARGIN_XLARGE
            right: parent.right
            rightMargin: UI.MARGIN_XLARGE
            bottom: parent.bottom
        }
        cacheBuffer: height
        delegate: categoryDelegate
        model: categoriesModel
        clip: true
        enabled: !pageStack.busy
        spacing: 5
    }


    AddCategoryDialog {
        id: addCatDialog
        titleText: qsTr("Add new category")
        catTitleText: qsTr("")
        rejectButtonText: qsTr("Cancel")
        acceptButtonText: qsTr("Add")
        onAccepted: window.addNewCategory(catTitleText)
    }
}
//![0]
