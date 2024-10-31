QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lWs2_32

SOURCES += \
    chat.cpp \
    ckernal.cpp \
    fileiteam.cpp \
    friendlist.cpp \
    groupchat.cpp \
    groupiteam.cpp \
    inet/TCPClient.cpp \
    inet/UDP.cpp \
    inet/inet.cpp \
    inetMediator/TCPClientMediator.cpp \
    inetMediator/inetMediator.cpp \
    main.cpp \
    chatdialog.cpp \
    useriteam.cpp

HEADERS += \
    chat.h \
    chatdialog.h \
    ckernal.h \
    fileiteam.h \
    friendlist.h \
    groupchat.h \
    groupiteam.h \
    inet/TCPClient.h \
    inet/UDP.h \
    inet/inet.h \
    inet/packDef.h \
    inetMediator/TCPClientMediator.h \
    inetMediator/inetMediator.h \
    useriteam.h

FORMS += \
    chat.ui \
    chatdialog.ui \
    fileiteam.ui \
    friendlist.ui \
    groupchat.ui \
    groupiteam.ui \
    useriteam.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
