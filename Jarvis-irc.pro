#-------------------------------------------------
#
# Project created by QtCreator 2012-07-31T11:54:06
#
#-------------------------------------------------

QT       += core network

#QT       -= gui
QMAKE_CXXFLAGS += -std=c++11
TARGET = Jarvis-irc
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../Jarvis/Frontend
LIBS += -L../Jarvis/Frontend/debug/ -lJarvis-Frontend

SOURCES += main.cpp \
    TerminalPrinter.cpp \
    IRC.cpp

HEADERS += \
    TerminalPrinter.h \
    IRC.h \
    InputWorker.h
