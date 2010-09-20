
# -----------------------------------------------------------------------------

QT -= core \
    gui

TARGET = gdf_merger
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

INCLUDEPATH += ../../libgdf/include
DEPENDPATH += . ../../libgdf/include

QMAKE_CXXFLAGS_WARN_ON = -Wall \
    -pedantic

# -----------------------------------------------------------------------------

SOURCES += main.cpp \
           gdfmerger.cpp

HEADERS += gdfmerger.h

# -----------------------------------------------------------------------------

LIBS += -lboost_filesystem \
        -lboost_system \
        -lboost_program_options \
        -L../../libgdf/build  -lGDF

# -----------------------------------------------------------------------------
