#-------------------------------------------------
#
# Project created by QtCreator 2014-11-27T12:59:04
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = fractal-compression
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    EasyBMP.cpp \
    fractalcompressor.cpp \
    fractal.cpp

HEADERS += \
    EasyBMP_DataStructures.h \
    EasyBMP_VariousBMPutilities.h \
    EasyBMP.h \
    EasyBMP_BMP.h \
    fractalcompressor.h \
    fractal.h
