#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
// #include <QStringList>

class QAction;
class QMenu;
class QSettings;
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
        void doViewPreferences();
        void setFrame(int value);

    private:
        QSettings* settings;
        QMap<QAction*, BaseView*> views;

        QString lastLocation;
        // QStringList recentFiles;

        DCD_Loader* dcd;
        Frame_Data* data;

        PlaybackControl* playbackControl;
        Renderer* renderer;
        ViewPreferenceDialog* viewPreferenceDialog;

        void setupMenu();
        void addRenderMode(BaseView* view, QMenu* menu);
};//MainWindow

#endif
