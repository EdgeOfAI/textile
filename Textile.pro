QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    formcontrol.cpp \
    main.cpp \
    mainwindow.cpp \
    videoprocess.cpp

HEADERS += \
    formcontrol.h \
    mainwindow.h \
    videoprocess.h

FORMS += \
    formcontrol.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


unix: !macx{
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

unix: macx{
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib \
    -lopencv_world
}


INCLUDEPATH += /usr/local/include/opencv4/opencv2

LIBS += `pkg-config --cflags --libs opencv`
LIBS += -L/home/cv-startup/projectAI/textile/lib -lhcnetsdk -lPlayCtrl -lAudioRender -lSuperRender
LIBS += -L/home/cv-startup/projectAI/textile/lib/HCNetSDKCom -lhcnetsdk


INCLUDEPATH += /home/cv-startup/projectAI/textile/include -lopencv_core -lopencv_videoio -lopencv_highgui
DEPENDPATH += /home/cv-startup/projectAI/textile/include

RESOURCES += \
    res.qrc
