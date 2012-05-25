TEMPLATE = app
QT += network declarative xml svg
TARGET = cutepress
VERSION = 1.3.3

INCLUDEPATH += src \
               src/wp

HEADERS += src/worker.h \
    src/wp/WPXmlStatsParser.h \
    src/wp/WPXmlRsdParser.h \
    src/wp/WPXmlResponse.h \
    src/wp/WPXmlRequest.h \
    src/wp/WPXmlApiKeyParser.h \
    src/wp/WPNetworkEngine.h \
    src/wp/WpMediaFile.h \
    src/wp/WPDataCustomField.h \
    src/wp/WPData.h \
    src/wp/version.h \
    src/wp/utils.h \
    src/wp/ScopedTimer.h \
    src/wp/md5.h \
    src/roleitemmodel.h \
    src/wp/MediaSearchThread.h \
    src/qexifimageheader.h \
    src/thumbnailthread.h \
    src/networkaccessmanagerfactory.h \
    src/cppost.h \
    src/cpblog.h \
    src/common.h

SOURCES += src/main.cpp \
           src/worker.cpp \
    src/wp/WPXmlStatsParser.cpp \
    src/wp/WPXmlRsdParser.cpp \
    src/wp/WPXmlResponse.cpp \
    src/wp/WPXmlRequest.cpp \
    src/wp/WPXmlApiKeyParser.cpp \
    src/wp/WPNetworkEngine.cpp \
    src/wp/WpMediaFile.cpp \
    src/wp/WPDataCustomField.cpp \
    src/wp/WPData.cpp \
    src/wp/utils.cpp \
    src/wp/UiData.cpp \
    src/wp/WPDataGravatars.cpp \
    src/wp/md5.cpp \
    src/wp/WPDataPost.cpp \
    src/wp/WPDataPage.cpp \
    src/wp/WPDataItem.cpp \
    src/wp/WPDataComment.cpp \
    src/roleitemmodel.cpp \
    src/wp/MediaSearchThread.cpp \
    src/qexifimageheader.cpp \
    src/thumbnailthread.cpp \
    src/networkaccessmanagerfactory.cpp

RESOURCES += cutepress.qrc

symbian {
    TARGET = CutePress
    TARGET.EPOCHEAPSIZE = 0x400000 0x6000000 #64MB
    TARGET.CAPABILITY += NetworkServices
    ICON = cutepress.svg
    export(ICON)
    CONFIG += qt-components
    DEPLOYMENT.display_name = CutePress
    RESOURCES += symbian.qrc
    supported_platforms = \
    "[0x20022E6D],0,0,0,{\"S60ProductID\"}"   # Only Symbian^3
    # remove default platforms
    default_deployment.pkg_prerules -= pkg_platform_dependencies
    # add our platforms
    platform_deploy.pkg_prerules += supported_platforms

    vendorinfo = "%{\"ShowStopper\"}" ":\"ShowStopper\""

    platform_deploy.pkg_prerules += vendorinfo

    DEPLOYMENT += platform_deploy

    INCLUDEPATH += middleware
    LIBS += \
        -lefsrv \
        -leikcore \
        -lavkon

}

win32 {
    RESOURCES += symbian.qrc \
                 meego.qrc
 }

OTHER_FILES += \
    qml/*.qml \
    qml/symbian/*.qml \
    qml/meego/*.qml \
    qml/*.js \
    qml/images/* \
    *.svg \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

contains(MEEGO_EDITION,harmattan) {
    RESOURCES += meego.qrc

    target.path = /opt/cutepress/bin
    INSTALLS += target

    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile

    icon.files = cutepress.png
    icon.path = /usr/share/icons/hicolor/80x80/apps
    INSTALLS += icon
}














