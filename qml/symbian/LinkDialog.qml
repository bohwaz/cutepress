/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import com.nokia.symbian 1.0
import com.nokia.extras 1.0

Dialog {
    id: root
    objectName: "root"

    property string titleText
    property int selectionStart
    property int selectionEnd
    property int cursorPos
    property alias linkTitleText: linkTitle.text
    property alias linkUrlText: link.text
    property alias acceptButtonText: acceptButton.text
    property alias rejectButtonText: rejectButton.text
    property alias icon: icon.source

    onStatusChanged: if (status == DialogStatus.Open) vertical.flash()

    title: Item {
        id: title
        height: platformStyle.graphicSizeSmall + 2 * platformStyle.paddingLarge
        width: parent.width

        Image {
            id: icon
            sourceSize.width: platformStyle.graphicSizeSmall
            sourceSize.height: platformStyle.graphicSizeSmall

            anchors {
                right: parent.right; rightMargin: icon.source != "" ? platformStyle.paddingLarge : 0
                verticalCenter: parent.verticalCenter
            }
        }

        Text {
            font { family: platformStyle.fontFamilyRegular; pixelSize: window.appGeneralFontSize-1 }
            color: platformStyle.colorNormalLink
            text: root.titleText
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight

            anchors {
                left: title.left; leftMargin: platformStyle.paddingLarge
                top: title.top; bottom: title.bottom
                right: icon.left; rightMargin: platformStyle.paddingLarge
            }
        }
    }

    content: Item {
        id: content
        height: internal.getContentAreaHeight()
        width: parent.width

        Item {
            Flickable {
                id: flickable
                width: parent.width
                height: parent.height
                anchors { left: parent.left; top: parent.top }
                contentHeight: grid.childrenRect.height
                contentWidth: width
                flickableDirection: Flickable.VerticalFlick
                clip: true
                interactive: grid.childrenRect.height > flickable.height

                Grid{
                    id: grid
                    columns: 2
                    anchors{
                        fill: parent
                        rightMargin: privateStyle.scrollBarThickness
                    }

                    Text {
                        width: 64
                        font { family: platformStyle.fontFamilyRegular; pixelSize: window.appGeneralFontSize-1 }
                        color: platformStyle.colorNormalLight
                        wrapMode: Text.Wrap
                        text: qsTr("URL")
                        anchors.verticalCenter: link.verticalCenter
                    }

                    TextField {
                        id: link
                        width: parent.width - 64 - privateStyle.scrollBarThickness
                        font.pixelSize: window.appInputFontSize
                        placeholderText: qsTr("Enter url here")
                        text: "http://"
                    }

                    Text {
                        width: 64
                        font { family: platformStyle.fontFamilyRegular; pixelSize: window.appGeneralFontSize-1 }
                        color: platformStyle.colorNormalLight
                        wrapMode: Text.Wrap
                        text: qsTr("Title")
                        anchors.verticalCenter: linkTitle.verticalCenter
                    }

                    TextField {
                        id: linkTitle
                        width: parent.width - 64 - privateStyle.scrollBarThickness
                        font.pixelSize: window.appInputFontSize
                        placeholderText: qsTr("Enter title here")
                    }
                }
            }

            ScrollBar {
                id: vertical
                height: parent.height
                anchors { top: flickable.top; right: flickable.right }
                flickableItem: flickable
                interactive: false
                orientation: Qt.Vertical
            }

            anchors {
                top: parent.top; topMargin: platformStyle.paddingLarge
                bottom: parent.bottom; bottomMargin: platformStyle.paddingLarge
                left: parent.left; leftMargin: platformStyle.paddingLarge
                right: parent.right
            }
        }
    }

    buttons: ToolBar {
        id: buttons
        width: parent.width
        height: privateStyle.toolBarHeightLandscape + 2 * platformStyle.paddingSmall
        tools: Row {
            id: buttonRow
            anchors.centerIn: parent
            spacing: platformStyle.paddingMedium

            Button {
                id: acceptButton
                font.pixelSize: window.appGeneralFontSize-1
                // Different widths for 1 and 2 button cases
                width: rejectButton.text == ""
                    ? Math.round((privateStyle.dialogMaxSize - 3 * platformStyle.paddingMedium) / 2)
                    : (buttons.width - 3 * platformStyle.paddingMedium) / 2
                onClicked: accept()
                visible: text != ""
            }
            Button {
                id: rejectButton
                font.pixelSize: window.appGeneralFontSize-1
                width: acceptButton.text == ""
                    ? Math.round((privateStyle.dialogMaxSize - 3 * platformStyle.paddingMedium) / 2)
                    : (buttons.width - 3 * platformStyle.paddingMedium) / 2
                onClicked: reject()
                visible: text != ""
            }
        }
    }

    QtObject {
        id: internal

        property int defaultContentHeight: root.height ? root.height - title.height - buttons.height
            : grid.childrenRect.height + 2 * platformStyle.paddingLarge

        function getContentAreaHeight() {
            // Constrain the default height within the bounds of the min and max heights
            return Math.max(Math.min(defaultContentHeight, platformContentMaximumHeight),
                privateStyle.dialogMinSize - title.height - buttons.height)
        }
    }
}
