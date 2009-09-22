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
#include "metaballs_view.h"
#include "pdb_loader.h"
#include "playback_control.h"
#include "point_view.h"
#include "renderer.h"
#include "view_preference.h"

MainWindow::MainWindow()
{
    lastLocation = new QString("");

    centralWidget = new QWidget(this);
    centralLayout = new QVBoxLayout(centralWidget);

    renderer = new Renderer(centralWidget);
    data = renderer->data;

    centralLayout->addWidget(renderer);

    playbackControl = new PlaybackControl(centralWidget);
    playbackControl->setTps(10);
    // connect(playbackControl, SIGNAL(tick()), this, SLOT(doTick()));
    connect(playbackControl, SIGNAL(frameChange(int)), this, SLOT(setFrame(int)));
    centralLayout->addWidget(playbackControl);

    setCentralWidget(centralWidget);
    resize(600, 480);

    dcd = NULL;
    viewPreferenceDialog = new ViewPreferenceDialog(this);

    //setup menu last since the renderer views depends on some stuff
    setupMenu();
    addRenderMode(new PointView());
    addRenderMode(new MetaballsView());
    renderer->setRenderMode(0);
    viewMenu->addSeparator();
    viewPreferencesAction = new QAction(tr("&View preferences"), viewMenu);
    viewPreferencesAction->setShortcut(tr("Ctrl+E"));
    connect(viewPreferencesAction, SIGNAL(triggered()), this, SLOT(doViewPreferences()));
    viewMenu->addAction(viewPreferencesAction);
}//constructor

MainWindow::~MainWindow()
{
    delete lastLocation;
    if (dcd != NULL)
        delete dcd;
    foreach (BaseView* view, views)
        delete view;
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
    playbackControl->setTotalFrames(dcd->totalFrames());
}//doOpenFile

void MainWindow::doTick()
{
    // not used?
    if (dcd == NULL)
        return;
    dcd->load_dcd_frame(data);
}//doTick

void MainWindow::doViewPreferences()
{
    int preferenceID = renderer->currentView()->preferenceID;
    if (preferenceID > -1)
        viewPreferenceDialog->setTabPage(preferenceID);

    foreach (BaseView* view, views)
        view->updatePreferences();
    viewPreferenceDialog->exec();
}//doViewPreferences

void MainWindow::setFrame(int value)
{
    if (dcd->frame(value))
    {
        dcd->load_dcd_frame(data);
        renderer->dataTick();
    }//if
}//setFrame


void MainWindow::setupMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    viewMenu = menuBar()->addMenu(tr("&View"));

    openFileAction = new QAction(tr("&Open image"), fileMenu);
    openFileAction->setShortcut(tr("Ctrl+O"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(doOpenFile()));

    toggleFocusPlaneAction = new QAction(tr("Toggle &zoom focus"), fileMenu);
    toggleFocusPlaneAction->setShortcut(tr("Tab"));
    connect(toggleFocusPlaneAction, SIGNAL(triggered()), renderer, SLOT(toggleFocusPlane()));

    quitAction = new QAction(tr("&Quit"), fileMenu);
    quitAction->setShortcut(tr("Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    // renderPointsAction = new QAction(tr("Render as &points"), viewMenu);
    // renderPointsAction->setShortcut(tr("Ctrl+1"));
    // connect(renderPointsAction, SIGNAL(triggered()), this, SLOT(doRenderPoints()));

    // renderMetaballsAction = new QAction(tr("Render as &metaballs"), viewMenu);
    // renderMetaballsAction->setShortcut(tr("Ctrl+2"));
    // connect(renderMetaballsAction, SIGNAL(triggered()), this, SLOT(doRenderMetaballs()));

    fileMenu->addAction(openFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(toggleFocusPlaneAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    // viewMenu->addAction(renderPointsAction);
    // viewMenu->addAction(renderMetaballsAction);
}//setupMenu


void MainWindow::addRenderMode(BaseView* view)
{
    QAction* viewAction = new QAction(view->viewName, viewMenu);
    int viewID = renderer->addRenderMode(view);
    if (viewID < 9) // shouldn't go beyond 9 for our system?
        viewAction->setShortcut(QString("Ctrl+%1").arg(viewID+1));
    connect(viewAction, SIGNAL(triggered()), view, SLOT(select()));
    connect(view, SIGNAL(selectView(int)), renderer, SLOT(setRenderMode(int)));
    viewMenu->addAction(viewAction);
    views[viewAction] = view;
    view->tick(data);

    int tabID = viewPreferenceDialog->addTab(view->preferenceWidget(), view->viewName);
    view->preferenceID = tabID;
    view->preferenceParent = viewPreferenceDialog;
}//addRenderMode

