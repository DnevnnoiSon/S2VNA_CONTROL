QT       += core gui widgets network charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += Inc

SOURCES += \
   Src/main.cpp \
   Src/mainwindow.cpp \
   Src/s2vna_scpi.cpp \
   Src/socketcommunication.cpp \
   Src/sparameterplotter.cpp

HEADERS += \
   Inc/ValidSettings.h \
   Inc/icommunication.h \
   Inc/mainwindow.h \
   Inc/s2vna_scpi.h \
   Inc/socketcommunication.h \
   Inc/sparameterplotter.h

FORMS += \
    ui/mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

