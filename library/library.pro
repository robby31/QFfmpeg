#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T16:19:25
#
#-------------------------------------------------

TARGET = library
TEMPLATE = lib

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    qffmpeg.pri

include (qffmpeg.pri)

