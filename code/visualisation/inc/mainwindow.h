#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QString;
class QVBoxLayout;

class DCD_Loader;
class Frame_Data;
class PlaybackControl;
class Renderer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

    private slots:
        void doOpenFile();
        void doTick();

        void setFrame(int value);

    private:
        QString* lastLocation;
        QWidget* centralWidget;
        QVBoxLayout* centralLayout;

        DCD_Loader* dcd;
        Frame_Data* data;
        PlaybackControl* playbackControl;
        Renderer* renderer;

        QMenu* fileMenu;
        QAction* openFileAction;
        QAction* toggleFocusPlaneAction;
        QAction* quitAction;

        void setupMenu();
};//MainWindow

#endif
