TEMPLATE = app
QT += opengl
DESTDIR = bin
TARGET = vis
DEPENDPATH += . src
INCLUDEPATH += . inc
MOC_DIR = .moc
OBJECTS_DIR = .obj

# Input
HEADERS += inc/mainwindow.h \
           inc/renderer.h \

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/renderer.cpp \
