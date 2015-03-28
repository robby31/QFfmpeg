#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T16:21:11
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_qffmpegtests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_qffmpegtests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DEFINES += PROFILING

HEADERS += \
    tst_qffmpegtests.h

include (../qffmpeg.pri)


