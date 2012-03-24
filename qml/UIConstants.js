/****************************************************************************
**
** Copyright (C) 2011 Abhishek Kumar
** All rights reserved.
** Contact: Abhishek Kumar (abhishek.mrt22@gmail.com)
**

**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions contained
** in the Technology Preview License Agreement accompanying this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Abhishek Kumar at abhishek.mrt22@gmail.com.
**
****************************************************************************/

.pragma library
var IS_THEME_LIGHT = false

var MARGIN_DEFAULT = 0;
var MARGIN_XLARGE = 10;
var SMALL_IMAGE_MAX_WIDTH = 240;
var SMALL_IMAGE_MAX_HEIGHT = SMALL_IMAGE_MAX_WIDTH;

var PAGE_HEADER_TITLE_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
var PAGE_HEADER_SEPARATOR = IS_THEME_LIGHT?"#005826":"#6dcff6";

var DETAILS_TOP_MARGIN_LANDSCAPE = 40;
var DETAILS_TOP_MARGIN_PORTRAIT = MARGIN_XLARGE;
var DETAILS_SPACING = 10;
var INFO_DETAILS_SPACING = 20;

var SMALLHEADINGTEXT_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
var HEADINGTEXT_COLOR = IS_THEME_LIGHT?"black":"white";
var BIGHEADINGTEXT_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";

var COMMENTSLIST_TOP_MARGIN = 10
var PROGRESSBAR_LABEL_BOTTOM_MARGIN = 4;
var PROGRESSBAR_LABEL_COLOR = "white";
var PROGRESSSTATUS_LABEL_COLOR = IS_THEME_LIGHT?"#a1a1a1":"#636363";

var SLIDER_BOTTOM_MARGIN = 65;
var SLIDER_SIDE_MARGIN = 25;
var SLIDER_OPACITY = 1;


var PAGE_BG_COLOR = "#101010"
var PAGE_BG_COLOR1 = "#eeeeee";
var PAGE_BG_OPACITY = 0.3;

var LARGEIMAGE_ERROR_MSG_COLOR = "white";

var UPLISTDELEGATE_SPACING = 5;
var LISTDELEGATE_SPACING = 2;
var UPLISTDELEGATE_SPACING = 2;
var LISTDELEGATE_MARGIN = 10;
var LISTDELEGATE_TITLE_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
var LISTDELEGATE_TITLE_COLOR_MARKED = "white";
var LISTDELEGATE_TEXT_COLOR = IS_THEME_LIGHT?"#363636":"#cccccc";
var LISTDELEGATE_TEXT_COLOR1 = "white";
var LISTDELEGATE_COMMENTSTATUS_COLOR0 = "orange";
var LISTDELEGATE_COMMENTSTATUS_COLOR1 = "green";
var LISTDELEGATE_COMMENTSTATUS_COLOR2 = "red";
var LISTDELEGATE_TEXT_COLOR_MARKED = "black";
var LISTDELEGATE_STYLE_COLOR = "black";
var LISTDELEGATE_STYLE_COLOR_MARKED = "#CCCCCC";
var LISTDELEGATE_BG_COLOR_PRESSED = IS_THEME_LIGHT?"#cccccc":"#363636";
var LISTDELEGATE_WRAPPER_COLOR = IS_THEME_LIGHT?"#cccccc":"#363636";
var LISTDELEGATE_WRAPPER_COLOR_MARKED = IS_THEME_LIGHT?"#3cb878":"#49aeda";

var GRIDVIEW_MARGIN = 5;

var LISTVIEW_MARGIN = 5;
var LISTVIEW_SPACING = 10;

var SEARCHBAR_HEIGHT = 40;
var SEARCHBAR_FONT = 12;
var SEARCHBAR_FONT_PIXELSIZE = SEARCHBAR_HEIGHT - 30;
var SEARCHBAR_MARGIN = 5;
var SEARCHBAR_BORDER_COLOR = "#b0b0b0";
var SEARCHBAR_BORDER_WIDTH = 3;
var SEARCHBAR_RADIUS = 5;

var LISTSECTION_LEFT_MARGIN = 0;
var LISTSECTION_RIGHT_MARGIN = 0;
var LISTSECTION_MARGINS = 0;
var LISTSECTION_TITLE_COLOR = IS_THEME_LIGHT?"#363636":"#cccccc";

var TOOLBUTTON_SMALL_WIDTH = 42;
var TOOLBUTTON_SMALL_HEIGHT = 42;

var BUSYINDICATOR_LARGE_SIDE = 64;


var BLOG_ITEMCOUNT_RECT = IS_THEME_LIGHT?"#005826":"#49aeda";
var BLOG_ITEMCOUNT_RECT_MARKED = "white"
var FONT_COLOR_BLUE = "#49aeda"

var BLOGIMG = IS_THEME_LIGHT?"qrc:/qml/images/blogs_black.png":"qrc:/qml/images/blogs.png";
var PUBLISHIMG = IS_THEME_LIGHT?"qrc:/qml/images/publish_black.png":"qrc:/qml/images/publish.png";
var MENUIMG = IS_THEME_LIGHT?"qrc:/qml/images/menu_black.png":"qrc:/qml/images/menu.png";
var SETTINGSIMG = IS_THEME_LIGHT?"qrc:/qml/images/settings_black.png":"qrc:/qml/images/settings.png";
//var INFOIMG = IS_THEME_LIGHT?"qrc:/qml/images/info_black.png":"qrc:/qml/images/info.png";
//var HELPIMG = IS_THEME_LIGHT?"qrc:/qml/images/help_black.png":"qrc:/qml/images/help.png";
//var QUITIMG = IS_THEME_LIGHT?"qrc:/qml/images/quit_black.png":"qrc:/qml/images/quit.png";
var EDITIMG = IS_THEME_LIGHT?"qrc:/qml/images/edit_black.png":"qrc:/qml/images/edit.png";
var DELETEIMG = IS_THEME_LIGHT?"qrc:/qml/images/delete_black.png":"qrc:/qml/images/delete.png";
var DELETEBIGIMG = IS_THEME_LIGHT?"qrc:/qml/images/recycle bin_black.png":"qrc:/qml/images/recycle bin.png";
var BACKIMG = IS_THEME_LIGHT?"qrc:/qml/images/back_black.png":"qrc:/qml/images/back.png";
var FORWARDIMG = IS_THEME_LIGHT?"qrc:/qml/images/forward_black.png":"qrc:/qml/images/forward.png";
var CLOSEIMG = IS_THEME_LIGHT?"qrc:/qml/images/close_black.png":"qrc:/qml/images/close.png";
var CLOSEBIGIMG = IS_THEME_LIGHT?"qrc:/qml/images/close_big_black.png":"qrc:/qml/images/close_big.png";
var DIRIMG = IS_THEME_LIGHT?"qrc:/qml/images/dir_black.png":"qrc:/qml/images/dir.png";
var ADDIMG = IS_THEME_LIGHT?"qrc:/qml/images/add_black.png":"qrc:/qml/images/add.png";
var ABORTTIMG = IS_THEME_LIGHT?"qrc:/qml/images/abort_black.png":"qrc:/qml/images/abort.png";
var RELOADIMG = IS_THEME_LIGHT?"qrc:/qml/images/reload_black.png":"qrc:/qml/images/reload.png";
var SAVEIMG = IS_THEME_LIGHT?"qrc:/qml/images/save_black.png":"qrc:/qml/images/save.png";
var COMMENTSIMG = IS_THEME_LIGHT?"qrc:/qml/images/comments_black.png":"qrc:/qml/images/comments.png"
var MEEGOCOMMENTSIMG = IS_THEME_LIGHT?"qrc:/qml/images/comments - meego_black.png":"qrc:/qml/images/comments - meego.png";

var UploadState = {
    None : 0,
    InQueue : 1,
    Uploading : 2,
    UploadError : 3,
    Uploaded : 4,
    InPhotosetQueue : 5,
    AddingToPhotoset : 6,
    AddToPhotosetError : 7,
    AddedToPhotoset: 8,
    Aborted: 9
};

var ProgressState = {
    None : 0,
    Success : 1,
    Processing : 2,
    Error : 3
}

function switchTheme() {
    IS_THEME_LIGHT = !IS_THEME_LIGHT

    PAGE_HEADER_TITLE_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
    PAGE_HEADER_SEPARATOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
    SMALLHEADINGTEXT_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
    HEADINGTEXT_COLOR = IS_THEME_LIGHT?"black":"white";
    BIGHEADINGTEXT_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
    PROGRESSSTATUS_LABEL_COLOR = IS_THEME_LIGHT?"#a1a1a1":"#636363";
    LISTDELEGATE_BG_COLOR_PRESSED = IS_THEME_LIGHT?"#cccccc":"#363636";
    LISTDELEGATE_WRAPPER_COLOR = IS_THEME_LIGHT?"#cccccc":"#363636";
    LISTDELEGATE_WRAPPER_COLOR_MARKED = IS_THEME_LIGHT?"#3cb878":"#49aeda";
    LISTDELEGATE_TITLE_COLOR = IS_THEME_LIGHT?"#005826":"#6dcff6";
    LISTDELEGATE_TEXT_COLOR = IS_THEME_LIGHT?"#464646":"#cccccc";
    LISTDELEGATE_TEXT_COLOR1 = IS_THEME_LIGHT?"#202020":"white";
    LISTSECTION_TITLE_COLOR = IS_THEME_LIGHT?"#363636":"#cccccc";
    BLOG_ITEMCOUNT_RECT = IS_THEME_LIGHT?"#005826":"#49aeda";

    BLOGIMG = IS_THEME_LIGHT?"qrc:/qml/images/blogs_black.png":"qrc:/qml/images/blogs.png";
    PUBLISHIMG = IS_THEME_LIGHT?"qrc:/qml/images/publish_black.png":"qrc:/qml/images/publish.png";
    MENUIMG = IS_THEME_LIGHT?"qrc:/qml/images/menu_black.png":"qrc:/qml/images/menu.png";
    SETTINGSIMG = IS_THEME_LIGHT?"qrc:/qml/images/settings_black.png":"qrc:/qml/images/settings.png";
    EDITIMG = IS_THEME_LIGHT?"qrc:/qml/images/edit_black.png":"qrc:/qml/images/edit.png";
    DELETEIMG = IS_THEME_LIGHT?"qrc:/qml/images/delete_black.png":"qrc:/qml/images/delete.png";
    DELETEBIGIMG = IS_THEME_LIGHT?"qrc:/qml/images/recycle bin_black.png":"qrc:/qml/images/recycle bin.png";
    BACKIMG = IS_THEME_LIGHT?"qrc:/qml/images/back_black.png":"qrc:/qml/images/back.png";
    FORWARDIMG = IS_THEME_LIGHT?"qrc:/qml/images/forward_black.png":"qrc:/qml/images/forward.png";
    CLOSEIMG = IS_THEME_LIGHT?"qrc:/qml/images/close_black.png":"qrc:/qml/images/close.png";
    ADDIMG = IS_THEME_LIGHT?"qrc:/qml/images/add_black.png":"qrc:/qml/images/add.png";
    ABORTTIMG = IS_THEME_LIGHT?"qrc:/qml/images/abort_black.png":"qrc:/qml/images/abort.png";
    RELOADIMG = IS_THEME_LIGHT?"qrc:/qml/images/reload_black.png":"qrc:/qml/images/reload.png";
    SAVEIMG = IS_THEME_LIGHT?"qrc:/qml/images/save_black.png":"qrc:/qml/images/save.png";
    COMMENTSIMG = IS_THEME_LIGHT?"qrc:/qml/images/comments_black.png":"qrc:/qml/images/comments.png"
    MEEGOCOMMENTSIMG = IS_THEME_LIGHT?"qrc:/qml/images/comments - meego_black.png":"qrc:/qml/images/comments - meego.png";
}
