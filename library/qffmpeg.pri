include (../../ffmpeg/ffmpeg.pri)
include (../../Analyzer/analyzer.pri)

QT       += gui

DEFINES += LIBRARY_LIBRARY

SOURCES += $$PWD/qffmpeg.cpp \
           $$PWD/qavframe.cpp

HEADERS += $$PWD/qffmpeg.h\
           $$PWD/library_global.h \
           $$PWD/qavframe.h



#win32:CONFIG(release, debug|release): LIBS += -L$$DESTDIR/release/ -llibrary
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR/debug/ -llibrary
#else:unix: LIBS += -L$$DESTDIR -llibrary

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
