#-------------------------------------------------
#
# Project created by Luigi Vigani
#
#-------------------------------------------------
include("$(ROOTSYS)/include/rootcint.pri")

QT       += core gui

TARGET = Tognazzi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    variableshandler.cpp \
    filereader.cpp \
    scurveanalysis.cpp \
    vtrimadjuster.cpp \
    vtrimgain.cpp \
    vtrimset.cpp \
    pixelselection.cpp \
    pixelsingle.cpp \
    umessanger.cpp \
    pixelselectionbutton.cpp \
    finetuner.cpp

HEADERS  += mainwindow.h \
    variableshandler.h \
    filereader.h \
    scurveanalysis.h \
    vtrimadjuster.h \
    vtrimgain.h \
    vtrimset.h \
    pixelselection.h \
    pixelsingle.h \
    umessanger.h \
    pixelselectionbutton.h \
    finetuner.h

FORMS    += mainwindow.ui \
    pixelselection.ui \
    pixelsingle.ui \
    finetuner.ui
