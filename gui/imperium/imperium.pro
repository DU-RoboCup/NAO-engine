#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T11:19:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imperium
TEMPLATE = app
CONFIG += c++11
LIBS += -L/home/paul/Documents/programming/RoboCup/RoboCupTestEnv/NAO/dev-tools/naoqi-sdk-2.1.4.13-linux64/lib \
        -lboost_system \
        -lboost_log \
        -lboost_log_setup \
        -lboost_thread
LIBS += -pthread
SOURCES += main.cpp\
        mainwindow.cpp \
        widgets/robot_view/robot_status_widget.cpp \
        widgets/robot_view/robotstatusbox.cpp \
        widgets/networking/connectwidget.cpp

HEADERS  += mainwindow.h \
    widgets/robot_view/robot_status_widget.h \
    widgets/robot_view/robotstatusbox.h \
    widgets/networking/connectwidget.h \
    networking/server.h \
    networking/payload.h

FORMS    += mainwindow.ui \
    widgets/robot_view/robot_status_widget.ui \
    widgets/robot_view/robotstatusbox.ui \
    widgets/networking/connectwidget.ui

SOURCES += \
    networking/server.cpp
