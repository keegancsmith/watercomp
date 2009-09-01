#include "mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QString>

#include <stdio.h>

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
    //loadData(*lastLocation);
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
