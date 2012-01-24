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

import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../UIConstants.js" as UI

Dialog {
    id: root
    objectName: "queryDialog"

    property alias titleText: myTitle.text
    property int selectionStart
    property int selectionEnd
    property int cursorPos
    property alias linkTitleText: linkTitle.text
    property alias linkUrlText: link.text

    //are they necessary?
    property alias acceptButtonText: acceptButton.text
    property alias rejectButtonText: rejectButton.text

    property Style platformStyle: QueryDialogStyle {}


  title: Text {
        id: myTitle
        width: parent.width

        font.family: root.platformStyle.titleFontFamily
        font.pixelSize: root.platformStyle.titleFontPixelSize
        font.bold:  root.platformStyle.titleFontBold
        font.capitalization: root.platformStyle.titleFontCapitalization
        elide: root.platformStyle.titleElideMode
        color: root.platformStyle.titleTextColor
        text: root.titleText
      }

  content:Item {
      id: myContent
      width: parent.width
      height: grid.childrenRect.height+grid.anchors.topMargin

            Grid{
                id: grid
                width: parent.width
                columns: 2
                spacing: 15
                anchors{
                    topMargin: root.platformStyle.contentTopMargin
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                Text {
                    width: 64
                    font.family: root.platformStyle.messageFontFamily
                    font.pixelSize: root.platformStyle.messageFontPixelSize
                    color: root.platformStyle.messageTextColor
                    wrapMode: Text.WordWrap
                    text: "URL"
                    anchors.verticalCenter: link.verticalCenter
                }

                TextField {
                    id: link
                    width: parent.width - 64 - parent.spacing
                    font.pixelSize: root.platformStyle.messageFontPixelSize
                    placeholderText: "Enter url here"
                    text: "http://"
                }

                Text {
                    width: 64
                    font.family: root.platformStyle.messageFontFamily
                    font.pixelSize: root.platformStyle.messageFontPixelSize
                    color: root.platformStyle.messageTextColor
                    wrapMode: Text.WordWrap
                    text: "Title"
                    anchors.verticalCenter: linkTitle.verticalCenter
                }

                TextField {
                    id: linkTitle
                    width: parent.width - 64 - parent.spacing
                    font.pixelSize: root.platformStyle.messageFontPixelSize
                    placeholderText: "Enter title here"
                }
            }
    }

  buttons: Grid {
              id: buttonCol
              anchors.top: parent.top
              anchors.topMargin: 15
              spacing: root.platformStyle.buttonsColumnSpacing
              columns: window.inPortrait?1:2

              height: (acceptButton.text  == "" ? 0 : acceptButton.height)
                      + (rejectButton.text == "" ? 0 : rejectButton.height)
                      + spacing

              anchors.horizontalCenter: parent.horizontalCenter

              Button {
                  id: acceptButton
                  text: ""
                  onClicked: accept()
                  visible: text != ""
                  __dialogButton: true                  
                  platformStyle: ButtonStyle { textColor: root.platformStyle.titleTextColor }
              }
              Button {
                  id: rejectButton
                  text: ""
                  onClicked: reject()
                  visible: text != ""
                  __dialogButton: true
                  platformStyle: ButtonStyle { textColor: root.platformStyle.titleTextColor }
              }
          }
}
