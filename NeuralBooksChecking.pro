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
    books_checker.cpp

HEADERS  += mainwindow.h \
    neuron.h \
    neuronet.h \
    exceptions.h \
    books_checker.h \
    logger.h \
    train_thread.h \
    neurolayer.h \
    graph_vis.h

FORMS    += mainwindow.ui

CONFIG += c++11
