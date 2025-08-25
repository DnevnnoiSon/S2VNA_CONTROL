QT += core gui widgets charts network

TARGET = Vector_Circuit_Analizator_Control
TEMPLATE = app
CONFIG += c++17

DESTDIR = build
MOC_DIR = $$DESTDIR/moc
OBJECTS_DIR = $$DESTDIR/obj
UI_DIR = $$DESTDIR/ui
RCC_DIR = $$DESTDIR/rcc


SOURCES += \
    src/app/main.cpp \
    src/core/caching/filecache.cpp \
    src/core/communication/socketcommunication.cpp \
    src/core/scpi/s2vnascpi.cpp \
    src/ui/mainwindow/mainwindow.cpp \
    src/ui/plotter/sparameterplotter.cpp \
    src/ui/toolpanel/toolpanel.cpp


HEADERS += \
    src/core/caching/filecache.h \
    src/core/communication/icommunication.h \
    src/core/communication/socketcommunication.h \
    src/core/scpi/s2vnascpi.h \
    src/core/settings/connectionSettings.h \
    src/ui/mainwindow/mainwindow.h \
    src/ui/plotter/sparameterplotter.h \
    src/ui/tools/itoolplugin.h \
    src/ui/toolpanel/toolpanel.h

FORMS += \
    src/ui/mainwindow/mainwindow.ui \
    src/ui/toolpanel/toolpanel.ui

RESOURCES += \
    resources/icons.qrc

INCLUDEPATH += $$PWD/src \
               $$PWD/src/core \
               $$PWD/src/core/caching \
               $$PWD/src/core/communication \
               $$PWD/src/core/scpi \
               $$PWD/src/core/settings \
               $$PWD/src/ui \
               $$PWD/src/ui/mainwindow \
               $$PWD/src/ui/plotter \
               $$PWD/src/ui/tools

unix {
    LIBS += -L/usr/lib/x86_64-linux-gnu
    # QMAKE_CXXFLAGS += -fPIC
}

win32 {
    CONFIG += console
}

CONFIG -= precompile_header
AUTOMOC = YES
