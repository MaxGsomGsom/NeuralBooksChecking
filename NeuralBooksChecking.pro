#-------------------------------------------------
#
# Project created by QtCreator 2017-04-09T21:38:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NeuralBooksChecking
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    layer.cpp \
    neuron.cpp \
    neuronet.cpp

HEADERS  += mainwindow.h \
    layer.h \
    neuron.h \
    neuronet.h

FORMS    += mainwindow.ui