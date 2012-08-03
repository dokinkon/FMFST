#-------------------------------------------------
#
# Project created by QtCreator 2012-06-29T08:25:50
#
#-------------------------------------------------

QT       += core gui

TARGET = FMFST
TEMPLATE = app


SOURCES +=  source/main.cpp\
            source/mainwindow.cpp \
            source/tableeditor.cpp \
            source/tabletabwidget.cpp \
            source/resourceeditor.cpp \
            source/pathweighteditor.cpp \
            source/canvas.cpp \
            source/nodeeditor.cpp \
            source/node.cpp \
            source/graphicsscene.cpp \
            source/nodeitem.cpp \
            source/connectionitem.cpp \
            source/fmfst.cpp \
            source/fmfstdialog.cpp \
            source/edge.cpp \
            source/graph.cpp \
    source/resultdialog.cpp \
    source/canvaswidget.cpp \
    source/createnodedialog.cpp

HEADERS  += source/mainwindow.h \
            source/tableeditor.h \
            source/tabletabwidget.h \
            source/resourceeditor.h \
            source/pathweighteditor.h \
            source/canvas.h \
            source/nodeeditor.h \
            source/node.h \
            source/graphicsscene.h \
            source/nodeitem.h \
            source/connectionitem.h \
            source/fmfst.h \
            source/fmfstdialog.h \
            source/edge.h \
            source/graph.h \
    source/resultdialog.h \
    source/canvaswidget.h \
    source/createnodedialog.h

FORMS    += form/mainwindow.ui \
            form/tableeditor.ui \
            form/tabletabwidget.ui \
            form/resourceeditor.ui \
            form/pathweighteditor.ui \
            form/nodeeditor.ui \
            form/fmfstdialog.ui \
    form/resultdialog.ui \
    source/createnodedialog.ui

RESOURCES += \
    FMFST.qrc
