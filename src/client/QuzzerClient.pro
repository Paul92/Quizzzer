#-------------------------------------------------
#
# Project created by QtCreator 2016-01-29T22:47:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QuzzerClient
TEMPLATE = app


SOURCES += main.cpp\
        quizzerclient.cpp \
    networkHandler.cpp

HEADERS  += quizzerclient.h \
    networkHandler.h

FORMS    += \
    login.ui
