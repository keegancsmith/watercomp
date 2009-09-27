#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QAction;
class QMenu;
class QString;
class QVBoxLayout;

class DCD_Loader;
class Frame_Data;

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
        void doTick();

        void doViewPreferences();

        void setFrame(int value);

    private:
        QMap<QAction*, BaseView*> views;

        QString* lastLocation;
        QWidget* centralWidget;
        QVBoxLayout* centralLayout;

        DCD_Loader* dcd;
        Frame_Data* data;

        PlaybackControl* playbackControl;
        Renderer* renderer;
        ViewPreferenceDialog* viewPreferenceDialog;

        QMenu* fileMenu;
        QAction* openFileAction;
        QAction* toggleFocusPlaneAction;
        QAction* quitAction;

        QMenu* viewMenu;
        QAction* renderPointsAction;
        QAction* renderMetaballsAction;
        QAction* viewPreferencesAction;

        void setupMenu();
        void addRenderMode(BaseView* view);
};//MainWindow

#endif
