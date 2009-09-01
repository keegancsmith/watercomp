#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QHBoxLayout;
class QMenu;
class QString;
class Renderer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        ~MainWindow();

    private slots:
        void doOpenFile();

    private:
        QString* lastLocation;
        QWidget* centralWidget;
        QHBoxLayout* centralLayout;
        Renderer* renderer;

        QMenu* fileMenu;
        QAction* openFileAction;
        QAction* quitAction;

        void setupMenu();
};//MainWindow

#endif
