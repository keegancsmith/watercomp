TEMPLATE = app
QT += opengl
DESTDIR = bin
TARGET = vis
DEPENDPATH += src
INCLUDEPATH += inc ../dcd_loader
MOC_DIR = .moc
OBJECTS_DIR = .obj

# Input
HEADERS += \
            inc/dcd_loader.h \
            inc/frame_data.h \
            inc/mainwindow.h \
            inc/renderer.h \
            inc/pdb_loader.h \

SOURCES += \
            src/dcd_loader.cpp \
            src/frame_data.cpp \
            src/main.cpp \
            src/mainwindow.cpp \
            src/renderer.cpp \
            src/pdb_loader.cpp \
