#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>

#include <vector>

#include <pdbio/AtomInformation.h>
#include <splitter/FrameSplitter.h>
#include <splitter/WaterMolecule.h>
#include <graph/gridgraphcreator/GridGraphCreator.h>

class QAction;
class QLabel;
class QMenu;
class QSettings;
class QSlider;
class QSpinBox;
class QVBoxLayout;

class DCDReader;
class Frame;
class QuantisedFrame;

class BaseView;
class PlaybackControl;
class Renderer;
class ViewPreferenceDialog;

class MetaballsView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

    private slots:
        void doOpenFile();
        void doViewPreferences();
        void setFrame(int value);
        void setTps(int value);
        void setQuantisationLevel(int value);

        void doProcessAllFrames();

    private:
        QSettings* settings;
        QMap<QAction*, BaseView*> views;

        int quantisationLevel;

        QString lastLocation;
        // QStringList recentFiles;

        std::vector<AtomInformation> pdb;
        DCDReader* dcdreader;
        float* atoms;
        int framenum;
        Frame* unquantised;
        QuantisedFrame* quantised;
        Frame* dequantised;

        PlaybackControl* playbackControl;
        Renderer* renderer;
        ViewPreferenceDialog* viewPreferenceDialog;
        QLabel* frameLabel;
        QSpinBox* quantisationSpinBox;
        QSlider* speedSlider;
        QLabel* speedLabel;

        MetaballsView* metaballsView;


        void setupMenu();
        void addRenderMode(BaseView* view, QMenu* menu);
};//MainWindow

#endif
