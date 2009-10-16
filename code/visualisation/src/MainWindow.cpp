#include "MainWindow.h"

#include <QAction>
#include <QCoreApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QVBoxLayout>

#include <string.h>

#include <pdbio/AtomInformation.h>
#include <pdbio/DCDReader.h>
#include <pdbio/Frame.h>
#include <pdbio/PDBReader.h>
#include <quantiser/QuantisedFrame.h>
#include <splitter/FrameSplitter.h>
#include <splitter/WaterMolecule.h>
#include <graph/gridgraphcreator/GridGraphCreator.h>

#include "BallStickView.h"
#include "ClusterView.h"
#include "MetaballsView.h"
#include "PlaybackControl.h"
#include "PointView.h"
#include "QuantiseErrorView.h"
#include "Renderer.h"
#include "ViewPreference.h"

MainWindow::MainWindow()
{
    QCoreApplication::setOrganizationName("Honours");
    QCoreApplication::setOrganizationDomain("www.cs.uct.ac.za");
    QCoreApplication::setApplicationName("Visualisation");
    settings = new QSettings;

    // recentFiles = settings->value("recentFiles").toStringList();
    // if (!recentFiles.isEmpty())
        // lastLocation = recentFiles.front();
    lastLocation = settings->value("lastFile", "").toString();

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);

    renderer = new Renderer(centralWidget);

    centralLayout->addWidget(renderer);

    playbackControl = new PlaybackControl(centralWidget);
    playbackControl->setTps(10);
    connect(playbackControl, SIGNAL(frameChange(int)), this, SLOT(setFrame(int)));
    centralLayout->addWidget(playbackControl);

    setCentralWidget(centralWidget);
    resize(600, 480);

    dcdreader = new DCDReader();
    atoms = NULL;
    viewPreferenceDialog = new ViewPreferenceDialog(this);

    //setup menu last since the renderer views depends on some stuff
    setupMenu();

    frame = 0;
    quantised = 0;
    dequantised = 0;
    renderer->setRenderMode(settings->value("Renderer/renderMode", 0).toInt());

    quantisationLevel = 5;
}//constructor

MainWindow::~MainWindow()
{
    settings->sync();
    delete settings;

    foreach (BaseView* view, views)
        delete view;
    delete dcdreader;
    if (atoms != NULL)
        delete [] atoms;
}//destructor


void MainWindow::doOpenFile()
{
    lastLocation = QFileDialog::getOpenFileName(this, tr("Open data"), lastLocation, tr("PDB files (*.pdb)"));
    if (lastLocation.isEmpty())
        return;
    // recentFiles.push_back(lastLocation);
    // while (recentFiles.size() > 5)
        // recentFiles.pop_back();
    // settings->setValue("recentFiles", recentFiles);
    settings->setValue("lastFile", lastLocation);

    pdb = load_pdbfile(lastLocation.toStdString().c_str());
    printf("pdb: %d\n", pdb.size());
    QString tFilename(lastLocation);
    dcdreader->open_file(tFilename.replace(QRegExp(".pdb$"), ".dcd").toStdString().c_str());
    playbackControl->setTotalFrames(dcdreader->nframes());

    __metaballs__->loadFile(tFilename.replace(QRegExp(".dcd$"), ".mbd").toStdString().c_str());

    foreach (BaseView* view, views)
    {
        view->init(pdb);
    }//foreach

    if (atoms) delete [] atoms;
    atoms = new float[3 * dcdreader->natoms()];
    if (frame) delete frame;
    frame = new Frame(atoms, dcdreader->natoms());
    setFrame(0);
    renderer->resetView(quantised->min_coord, quantised->max_coord);
    renderer->currentView()->select();
}//doOpenFile

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
    dcdreader->set_frame(value);
    if (!dcdreader->next_frame(*frame))
    {
        printf("DCDReader error\n");
        return;
    }//if

    if (quantised != NULL) delete quantised;
    quantised = new QuantisedFrame(*frame, quantisationLevel, quantisationLevel, quantisationLevel);
    if (dequantised != NULL) delete dequantised;
    dequantised = new Frame(quantised->toFrame());
    renderer->dataTick(value, frame, quantised, dequantised);
}//setFrame


void MainWindow::setupMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* openFileAction = new QAction(tr("&Open image"), fileMenu);
    openFileAction->setShortcut(tr("Ctrl+O"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(doOpenFile()));

    QAction* toggleFocusPlaneAction = new QAction(tr("Toggle &zoom focus"), fileMenu);
    toggleFocusPlaneAction->setShortcut(tr("Tab"));
    connect(toggleFocusPlaneAction, SIGNAL(triggered()), renderer, SLOT(toggleFocusPlane()));

    QAction* quitAction = new QAction(tr("&Quit"), fileMenu);
    quitAction->setShortcut(tr("Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(openFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(toggleFocusPlaneAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    addRenderMode(new PointView(), viewMenu);
    __metaballs__ = new MetaballsView();
    addRenderMode(__metaballs__, viewMenu);
    addRenderMode(new ClusterView(), viewMenu);
    addRenderMode(new QuantiseErrorView(), viewMenu);
    addRenderMode(new BallStickView(), viewMenu);

    QAction* __process__all__frames__action__ = new QAction(tr("Process all frames for Metaballs view"), fileMenu);
    connect(__process__all__frames__action__, SIGNAL(triggered()), this, SLOT(__do__process__all__frames__()));
    fileMenu->addSeparator();
    fileMenu->addAction(__process__all__frames__action__);

    viewMenu->addSeparator();
    QAction* viewPreferencesAction = new QAction(tr("&View preferences"), viewMenu);
    viewPreferencesAction->setShortcut(tr("Ctrl+E"));
    connect(viewPreferencesAction, SIGNAL(triggered()), this, SLOT(doViewPreferences()));
    viewMenu->addAction(viewPreferencesAction);
}//setupMenu


void MainWindow::addRenderMode(BaseView* view, QMenu* menu)
{
    QAction* viewAction = new QAction(view->viewName, menu);
    int viewID = renderer->addRenderMode(view);
    if (viewID < 9) // shouldn't go beyond 9 for our system?
        viewAction->setShortcut(QString("Ctrl+%1").arg(viewID+1));
    connect(viewAction, SIGNAL(triggered()), view, SLOT(select()));
    connect(view, SIGNAL(selectView(int)), renderer, SLOT(setRenderMode(int)));
    menu->addAction(viewAction);
    views[viewAction] = view;
    view->parent = renderer;

    int tabID = viewPreferenceDialog->addTab(view->preferenceWidget(), view->viewName);
    view->preferenceID = tabID;
    view->preferenceParent = viewPreferenceDialog;
}//addRenderMode



void MainWindow::__do__process__all__frames__()
{
    QString mbdFilename(lastLocation);
    __metaballs__->processVolume(mbdFilename.replace(QRegExp(".pdb$"), ".mbd").toStdString().c_str(), dcdreader);
}//__do__process__all__frames__

