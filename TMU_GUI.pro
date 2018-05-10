#-------------------------------------------------
#
# Project created by QtCreator 2018-04-19T14:44:35
#
#-------------------------------------------------

QT       += core gui serialport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TMU_GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    hxusbcomm.cpp \
    hxusbcommworker.cpp \
    utils.cpp \
    tmu.cpp \
    hxutils.cpp \
    savemyui.cpp \
    tmudaemon.cpp \
    tmutempcycle.cpp

HEADERS  += mainwindow.h \
    hx_commstd.h \
    hxusbcomm.h \
    hxusbcommworker.h \
    utils.h \
    tmu.h \
    tmu_std.h \
    tmu_gui_properties.h \
    hxutils.h \
    hxguiproperties.h \
    savemyui.h \
    tmudaemon.h \
    tmutempcycle.h

win32:LIBS += "$$PWD/ftd2xx.lib"

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

DISTFILES += \
    TO_DO.txt \
    ftd2xx.lib \
    ftd2xx64.dll \
    SaveMyUI_README.txt \
    TestingComm.xml \
    ConfigFileNames.txt \
    build-TMU_GUI-Desktop_Qt_5_8_0_MinGW_32bit-Debug/Temperature_Profile_0.xml

RESOURCES += \
    tmu_resources.qrc

