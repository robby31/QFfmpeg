SOURCES += $$PWD/qffmpegprocess.cpp \
           #$$PWD/qffmpeg.cpp \
           #$$PWD/qavframe.cpp

HEADERS += $$PWD/qffmpegprocess.h\
           #$$PWD/qffmpeg.h \
           #$$PWD/qavframe.h

#include (../ffmpeg/ffmpeg.pri)
include (../Analyzer/analyzer.pri)


INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
