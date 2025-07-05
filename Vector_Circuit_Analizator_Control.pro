QT       += core gui widgets network charts

CONFIG += c++17

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += Inc

SOURCES += \
   Src/filecache.cpp \
   Src/main.cpp \
   Src/mainwindow.cpp \
   Src/s2vna_scpi.cpp \
   Src/socketcommunication.cpp \
   Src/sparameterplotter.cpp

HEADERS += \
   Inc/connectionSettings.h \
   Inc/icommunication.h \
   Inc/mainwindow.h \
   Inc/s2vna_scpi.h \
   Inc/socketcommunication.h \
   Inc/sparameterplotter.h \
   Inc/filecache.h

FORMS += \
    ui/mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

