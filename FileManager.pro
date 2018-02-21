#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T17:04:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        zip/src/zip.c \
    zipthread.cpp

HEADERS  += mainwindow.h \
        zip/src/zip.h \
        zip/src/miniz.h \
    zipthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
