#include "mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QString>

#include <string.h>

#include "frame_data.h"
#include "pdb_loader.h"
#include "renderer.h"

MainWindow::MainWindow()
{
    setupMenu();
    lastLocation = new QString("");
    centralWidget = new QWidget(this);
    centralLayout = new QHBoxLayout(centralWidget);

    renderer = new Renderer(this);

    centralLayout->addWidget(renderer);
    setCentralWidget(centralWidget);
    resize(600, 480);
}//constructor

MainWindow::~MainWindow()
{
    delete lastLocation;
}//destructor

void MainWindow::doOpenFile()
{
    *lastLocation = QFileDialog::getOpenFileName(this, tr("Open data"), *lastLocation, tr("All (*.*)"));
    if (lastLocation->isEmpty())
        return;

    PDB_Loader l;
    renderer->data->clear();
    l.load_file(lastLocation->toStdString().c_str(), renderer->data);
    renderer->data->update_bbox();
    renderer->resetView();
    renderer->updateGL();
}//doOpenFile

void MainWindow::setupMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    openFileAction = new QAction(tr("&Open image"), fileMenu);
    openFileAction->setShortcut(tr("Ctrl+O"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(doOpenFile()));

    quitAction = new QAction(tr("&Quit"), fileMenu);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(openFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
}//setupMenu