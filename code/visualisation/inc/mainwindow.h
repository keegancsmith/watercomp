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

    private:
        QSettings* settings;
        QMap<QAction*, BaseView*> views;

        QString lastLocation;
        // QStringList recentFiles;

        std::vector<AtomInformation> pdb;
        DCDReader* dcdreader;
        float* atoms;
        Frame* frame;
        QuantisedFrame* data;

        PlaybackControl* playbackControl;
        Renderer* renderer;
        ViewPreferenceDialog* viewPreferenceDialog;

        void setupMenu();
        void addRenderMode(BaseView* view, QMenu* menu);
};//MainWindow

#endif
