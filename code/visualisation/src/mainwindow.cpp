#include "mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QRegExp>
#include <QString>
#include <QVBoxLayout>

#include <string.h>

#include "dcd_loader.h"
#include "frame_data.h"
#include "pdb_loader.h"
#include "playback_control.h"
#include "renderer.h"

MainWindow::MainWindow()
{
    lastLocation = new QString("");

    centralWidget = new QWidget(this);
    centralLayout = new QVBoxLayout(centralWidget);

    renderer = new Renderer(centralWidget);
    data = renderer->data;
    centralLayout->addWidget(renderer);

    playbackControl = new PlaybackControl(centralWidget);
    playbackControl->tps(10);
    connect(playbackControl, SIGNAL(tick()), this, SLOT(doTick()));
    connect(playbackControl, SIGNAL(frameChange(int)), this, SLOT(setFrame(int)));
    centralLayout->addWidget(playbackControl);

    setupMenu();
    setCentralWidget(centralWidget);
    resize(600, 480);

    dcd = NULL;
}//constructor

MainWindow::~MainWindow()
{
    delete lastLocation;
    if (dcd != NULL)
        delete dcd;
}//destructor


void MainWindow::doOpenFile()
{
    *lastLocation = QFileDialog::getOpenFileName(this, tr("Open data"), *lastLocation, tr("All (*.*)"));
    if (lastLocation->isEmpty())
        return;

    PDB_Loader l;
    data->clear();
    if (!l.load_file(lastLocation->toStdString().c_str(), data))
        return;
    data->update_bbox();
    renderer->resetView();

    if (dcd == NULL)
        delete dcd;
    dcd = new DCD_Loader();
    dcd->load_dcd_file(lastLocation->replace(QRegExp(".pdb$"), ".dcd").toStdString().c_str());
    playbackControl->totalFrames(dcd->totalFrames());
}//doOpenFile

void MainWindow::doTick()
{
    if (dcd == NULL)
        return;
    dcd->load_dcd_frame(data);
}//doTick

void MainWindow::setFrame(int value)
{
    if (dcd->frame(value))
        dcd->load_dcd_frame(data);
}//setFrame


void MainWindow::setupMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    openFileAction = new QAction(tr("&Open image"), fileMenu);
    openFileAction->setShortcut(tr("Ctrl+O"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(doOpenFile()));

    toggleFocusPlaneAction = new QAction(tr("Toggle &zoom focus"), fileMenu);
    toggleFocusPlaneAction->setShortcut(tr("Tab"));
    connect(toggleFocusPlaneAction, SIGNAL(triggered()), renderer, SLOT(toggleFocusPlane()));

    quitAction = new QAction(tr("&Quit"), fileMenu);
    quitAction->setShortcut(tr("Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(openFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(toggleFocusPlaneAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
}//setupMenu

