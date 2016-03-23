#-------------------------------------------------
#
# Project created by QtCreator 2015-07-12T17:01:39
#
#-------------------------------------------------

QT       += core gui

LIBS += -llept -ltesseract `pkg-config opencv --libs`

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudokusolver
TEMPLATE = app


SOURCES += main.cpp\
           sudokusolver.cpp \
           sudokuengine.cpp \
           iogrid.cpp

HEADERS  += sudokusolver.h \
            sudokuengine.h \
            iogrid.h
