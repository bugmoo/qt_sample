QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tcp_client.cpp \
    tcp_client_win.cpp \
    tcp_server.cpp \
    tcp_server_win.cpp \
    udp.cpp \
    udp_win.cpp

HEADERS += \
    mainwindow.h \
    tcp_client.h \
    tcp_client_win.h \
    tcp_server.h \
    tcp_server_win.h \
    udp.h \
    udp_win.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
