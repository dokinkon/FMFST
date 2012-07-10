#-------------------------------------------------
#
# Project created by QtCreator 2012-06-29T08:25:50
#
#-------------------------------------------------

QT       += core gui

TARGET = PathFinder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tableeditor.cpp \
    tabletabwidget.cpp \
    resourceeditor.cpp \
    pathweighteditor.cpp \
    canvas.cpp \
    nodeeditor.cpp \
    node.cpp \
    graphicsscene.cpp \
    nodeitem.cpp \
    connectionitem.cpp \
    fmfst.cpp \
    fmfstdialog.cpp \
    edge.cpp \
    graph.cpp

HEADERS  += mainwindow.h \
    tableeditor.h \
    tabletabwidget.h \
    resourceeditor.h \
    pathweighteditor.h \
    canvas.h \
    nodeeditor.h \
    node.h \
    graphicsscene.h \
    nodeitem.h \
    connectionitem.h \
    fmfst.h \
    fmfstdialog.h \
    edge.h \
    graph.h

FORMS    += mainwindow.ui \
    tableeditor.ui \
    tabletabwidget.ui \
    resourceeditor.ui \
    pathweighteditor.ui \
    nodeeditor.ui \
    fmfstdialog.ui
