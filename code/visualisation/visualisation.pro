TEMPLATE = app
QT += opengl
LIBS += -lgts
DESTDIR = bin
TARGET = vis
DEPENDPATH += src
INCLUDEPATH += inc ../dcd_loader /usr/include/glib-2.0 /usr/lib/glib-2.0/include
MOC_DIR = .moc
OBJECTS_DIR = .obj

# Input
HEADERS += \
            inc/base_view.h \
            inc/dcd_loader.h \
            inc/frame_data.h \
            inc/mainwindow.h \
            inc/metaballs_view.h \
            inc/renderer.h \
            inc/pdb_loader.h \
            inc/playback_control.h \
            inc/point_view.h \
            inc/quaternion.h \
            inc/view_preference.h \

SOURCES += \
            src/base_view.cpp \
            src/dcd_loader.cpp \
            src/frame_data.cpp \
            src/main.cpp \
            src/mainwindow.cpp \
            src/metaballs_view.cpp \
            src/renderer.cpp \
            src/pdb_loader.cpp \
            src/playback_control.cpp \
            src/point_view.cpp \
            src/quaternion.cpp \
            src/view_preference.cpp \
