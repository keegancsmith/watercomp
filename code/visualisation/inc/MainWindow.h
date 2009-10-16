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
class QMenu;
class QSettings;
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

        void __do__process__all__frames__();

    private:
        QSettings* settings;
        QMap<QAction*, BaseView*> views;

        int quantisationLevel;

        QString lastLocation;
        // QStringList recentFiles;

        std::vector<AtomInformation> pdb;
        DCDReader* dcdreader;
        float* atoms;
        Frame* frame;
        QuantisedFrame* quantised;
        Frame* dequantised;

        PlaybackControl* playbackControl;
        Renderer* renderer;
        ViewPreferenceDialog* viewPreferenceDialog;

        MetaballsView* __metaballs__;


        void setupMenu();
        void addRenderMode(BaseView* view, QMenu* menu);
};//MainWindow

#endif
