#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T11:19:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imperium
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    widgets/network/connect.cpp

HEADERS  += mainwindow.h \
    widgets/network/connect.h

FORMS    += mainwindow.ui \
    widgets/network/connect.ui
