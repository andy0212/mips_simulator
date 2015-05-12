#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T15:31:22
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11
TARGET = assembler
TEMPLATE = app


SOURCES +=\
    assembler.cpp \
    binarytoasm.cpp \
    ConverToBinary.cpp \
    Simulator.cpp \
    main.cpp

HEADERS  += \
    assembler.h \
    binarytoasm.h \
    convertobinary.h \
    Simulator.h

FORMS    += mainwindow.ui \
    assembler.ui \
    ConsoleDialog.ui \
    setRegDialog.ui

DISTFILES +=
