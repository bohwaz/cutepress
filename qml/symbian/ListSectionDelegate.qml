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

import QtQuick 1.0
import com.nokia.symbian 1.0
import "../UIConstants.js" as UI

Item {
    property alias sectionName: sectionDelegateText.text
    property string color: UI.LISTSECTION_TITLE_COLOR
    property int margins: 10

    id: sectionDelegate

    anchors { left: parent.left; right: parent.right }

    height: sectionDelegateText.height + 6

    Rectangle {
        id: sectionDelegateDivider
        anchors.leftMargin: margins
        anchors.left: parent.left
        anchors.rightMargin: 5
        anchors.right: sectionDelegateText.left
        height: 1
        color: sectionDelegate.color
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: sectionDelegateText
        color: sectionDelegate.color
        font.pixelSize: window.appSectionFontSize;
        font.weight: Font.Normal
        anchors.right: parent.right
        anchors.rightMargin: margins
        anchors.verticalCenter: sectionDelegateDivider.verticalCenter
        elide: Text.ElideRight
    }
}
