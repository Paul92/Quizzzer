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
    networkHandler.cpp \
    login.cpp \
    question.cpp \
    waitWindow.cpp \
    questionData.cpp \
    scoreTable.cpp \
    endgame.cpp

HEADERS  += \
    networkHandler.h \
    login.h \
    question.h \
    ui.h \
    waitWindow.h \
    questionData.h \
    scoreTable.h \
    endgame.h

FORMS    += \
    login.ui \
    question.ui \
    wait.ui \
    end.ui
