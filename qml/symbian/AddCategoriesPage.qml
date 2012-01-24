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
            onClicked: {
                window.setCategoriesToPost("")
                pageStack.pop()
            }
        }
        ToolButton {
            id: button2
            flat: true
            iconSource: "qrc:/qml/images/add.png"
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
                text: window.addCategoryState==2?window.addCategoryStatus:"Select Category"
                width: status.visible?parent.width-parent.spacing-status.width:parent.width
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                color: UI.PAGE_HEADER_TITLE_COLOR
            }

            BusyIndicator {
                id: status
                visible: window.addCategoryState==1
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                width: 32
                height: width
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
                color: isMarked?UI.LISTDELEGATE_TITLE_COLOR_MARKED:UI.LISTDELEGATE_TITLE_COLOR
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
        titleText: "Add new category"
        catTitleText: ""
        rejectButtonText: "Cancel"
        acceptButtonText: "Add"
        //onRejected:
        onAccepted: window.addNewCategory(catTitleText)
    }
}
//![0]
