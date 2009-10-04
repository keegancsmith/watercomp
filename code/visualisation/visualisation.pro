TEMPLATE = app
QT += opengl
LIBS += -lgts -L../ann -lANN
DESTDIR = bin
TARGET = vis
DEPENDPATH += src
INCLUDEPATH += inc \
    ../dcd_loader \
    ../ann \
    /usr/include/glib-2.0 \
    /usr/lib/glib-2.0/include \
    ../components \
    ../components/quantiser \
    ../components/pdbio \
    ../components/splitter \

MOC_DIR = .obj
OBJECTS_DIR = .obj

# Input
HEADERS += \
            inc/base_view.h \
            inc/cluster_view.h \
            inc/mainwindow.h \
            inc/metaballs_view.h \
            inc/renderer.h \
            inc/playback_control.h \
            inc/point_view.h \
            inc/quaternion.h \
            inc/view_preference.h \

SOURCES += \
            src/base_view.cpp \
            src/cluster_view.cpp \
            src/main.cpp \
            src/mainwindow.cpp \
            src/metaballs_view.cpp \
            src/renderer.cpp \
            src/playback_control.cpp \
            src/point_view.cpp \
            src/quaternion.cpp \
            src/view_preference.cpp \
            ../components/graph/Graph.cpp \
            ../components/graph/GraphCreator.cpp \
            ../components/graph/SpanningTree.cpp \
            ../components/graph/anngraphcreator/ANNGraphCreator.cpp \
            ../components/graph/gridgraphcreator/GridGraphCreator.cpp \
            ../components/pdbio/AtomInformation.cpp \
            ../components/pdbio/DCDReader.cpp \
            ../components/pdbio/Frame.cpp \
            ../components/pdbio/PDBReader.cpp \
            ../components/quantiser/QuantisedFrame.cpp \
            ../components/splitter/FrameSplitter.cpp \
            ../components/splitter/WaterMolecule.cpp \

