TEMPLATE = app
CONFIG += debug
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
            inc/BallStickView.h \
            inc/BaseView.h \
            inc/ClusterView.h \
            inc/MainWindow.h \
            inc/MetaballsView.h \
            inc/Renderer.h \
            inc/PlaybackControl.h \
            inc/PointView.h \
            inc/QuantiseErrorView.h \
            inc/Quaternion.h \
            inc/Util.h \
            inc/ViewPreference.h \

SOURCES += \
            src/BallStickView.cpp \
            src/BaseView.cpp \
            src/ClusterView.cpp \
            src/main.cpp \
            src/MainWindow.cpp \
            src/MetaballsView.cpp \
            src/Renderer.cpp \
            src/PlaybackControl.cpp \
            src/QuantiseErrorView.cpp \
            src/PointView.cpp \
            src/Quaternion.cpp \
            src/Util.cpp \
            src/ViewPreference.cpp \
            ../components/graph/Graph.cpp \
            ../components/graph/anngraphcreator/ANNGraphCreator.cpp \
            ../components/graph/gridgraphcreator/GridGraphCreator.cpp \
            ../components/pdbio/AtomInformation.cpp \
            ../components/pdbio/DCDReader.cpp \
            ../components/pdbio/Frame.cpp \
            ../components/pdbio/PDBReader.cpp \
            ../components/quantiser/QuantisedFrame.cpp \
            ../components/splitter/FrameSplitter.cpp \
            ../components/splitter/WaterMolecule.cpp \

