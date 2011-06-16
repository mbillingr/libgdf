# -------------------------------------------------
# Project created by QtCreator 2010-07-28T14:59:34
# -------------------------------------------------
QT -= core \
    gui
TARGET = gdftest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
INCLUDEPATH += ../../libgdf/include
QMAKE_CXXFLAGS_WARN_ON = -Wall \
    -pedantic
SOURCES += main.cpp \
    ../../libgdf/src/Channel.cpp \
    ../../libgdf/src/SignalHeader.cpp \
    ../../libgdf/src/Record.cpp \
    ../../libgdf/src/RecordBuffer.cpp \
    ../../libgdf/src/Writer.cpp \
    ../../libgdf/src/GDFHeaderAccess.cpp \
    ../../libgdf/src/MainHeader.cpp \
    ../../libgdf/src/EventHeader.cpp \
    ../../libgdf/src/Reader.cpp \
    ../../libgdf/src/Types.cpp \
    ../../libgdf/src/Modifier.cpp \
    ../../libgdf/src/EventConverter.cpp
HEADERS += ../../libgdf/include/GDF/Types.h \
    ../../libgdf/include/GDF/SignalHeader.h \
    ../../libgdf/include/GDF/Exceptions.h \
    ../../libgdf/include/GDF/ChannelDataBase.h \
    ../../libgdf/include/GDF/ChannelData.h \
    ../../libgdf/include/GDF/Channel.h \
    ../../libgdf/include/GDF/HeaderItem.h \
    ../../libgdf/include/GDF/Record.h \
    ../../libgdf/include/GDF/GDFHeaderAccess.h \
    ../../libgdf/include/GDF/TagHeader.h \
    ../../libgdf/include/GDF/MainHeader.h \
    ../../libgdf/include/GDF/RecordBuffer.h \
    ../../libgdf/include/GDF/Writer.h \
    ../../libgdf/include/GDF/tools.h \
    ../../libgdf/include/GDF/EventHeader.h \
    ../../libgdf/include/GDF/Reader.h \
    ../../libgdf/include/GDF/Modifier.h \
    ../../libgdf/include/GDF/pointerpool.h \
    ../../libgdf/include/GDF/EventConverter.h
