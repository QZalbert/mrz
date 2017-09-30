#-------------------------------------------------
#
# Project created by QtCreator 2017-09-22T15:10:33
#
#-------------------------------------------------

QT       += core gui

TARGET = markWidget
#TEMPLATE = lib

CONFIG += qt thread dll


DEFINES += ULAPI

INCLUDEPATH += . \
    ./configs \
    ./device \
    ./UI \
    ./algorithm \
    /usr/local/include/opencv \
    /home/u/hbc/project/cnc/include \

VERSION = 1.1.2

LIBS += ./device/libQzCamera.so.1.0.0


LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_ml -lopencv_highgui\
        -L/home/u/hbc/project/cnc/lib/ -lemc -lnml -lemcini -lemchal \
        /usr/lib/libCncWidget.so.1.0.0 /usr/lib/libpvcnc.so /usr/lib/libpvhal.so \
        /usr/lib/libPythonQt.so.1.0.0 /usr/lib/libPythonQt_QtAll.so.1.0.0


SOURCES += main.cpp\
        markwidget.cpp \
    UI/viewQt.cpp \
    configs/param.cpp \
    configs/markparam.cpp \
    configs/iniwrite.cc \
    UI/markview.cpp \
    UI/focusview.cpp \
    UI/patternedit.cpp \
    UI/patterneditview.cpp \
    algorithm/nccMatch.cpp \
    algorithm/markdetect.cpp \
    algorithm/imggeom.cpp \
    device/markcnc.cpp \
    markWidget2.cpp \
    UI/pswdialog.cpp

HEADERS  += markwidget.h \
    UI/viewQt.h \
    configs/param.h \
    configs/markparam.h \
    configs/iniwrite.hh \
    device/camera.h \
    global.h \
    UI/markview.h \
    UI/focusview.h \
    UI/patternedit.h \
    UI/patterneditview.h \
    algorithm/nccMatch.h \
    algorithm/markdetect.h \
    algorithm/imggeom.h \
    device/cnc.h \
    device/markcnc.h \
    markWidget2.h \
    UI/pswdialog.h

FORMS    += markwidget.ui \
    UI/patternedit.ui \
    UI/pswDialog.ui \
    UI/pswdialog.ui

OTHER_FILES += \
    update_package/update.sh \
    create_update.sh \
    update_package/share/versionInfo.txt \
    update_package/configs/mark.hal
