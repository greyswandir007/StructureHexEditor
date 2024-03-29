#-------------------------------------------------
#
# Project created by QtCreator 2021-02-08T15:45:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StructureHexEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        hexeditorwindow.cpp \
    hexeditor.cpp \
    hexmanager.cpp \
    hexmodel.cpp \
    components/propertiesdialog.cpp \
    components/propertiesmodel.cpp \
    components/propertydelegate.cpp \
    structureeditor.cpp \
    components/jsontreemodel.cpp \
    components/jsonstoreddata.cpp \
    components/structurebytearray.cpp \
    components/jsonstoreddatahelper.cpp \
    components/mainview.cpp \
    palettedialog.cpp \
    components/jsonhighlighter.cpp \
    gotodialog.cpp \
    finddialog.cpp

HEADERS += \
        hexeditorwindow.h \
    hexeditor.h \
    hexmanager.h \
    hexmodel.h \
    components/propertiesdialog.h \
    components/propertiesmodel.h \
    components/propertydelegate.h \
    structureeditor.h \
    components/jsontreemodel.h \
    components/jsonstoreddata.h \
    components/structurebytearray.h \
    components/jsonstoreddatahelper.h \
    components/mainview.h \
    palettedialog.h \
    components/imagepreviewparams.h \
    components/structurenameditem.h \
    components/jsonhighlighter.h \
    gotodialog.h \
    finddialog.h

FORMS += \
        hexeditorwindow.ui \
    hexeditor.ui \
    components/propertiesdialog.ui \
    structureeditor.ui \
    palettedialog.ui \
    gotodialog.ui \
    finddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    structurehexeditor.qrc
