#include "ClusterView.h"

#include <cstdio>
#include <map>
#include <vector>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <GL/gl.h>

#include <pdbio/AtomInformation.h>
#include <pdbio/DCDReader.h>
#include <pdbio/Frame.h>
#include <pdbio/PDBReader.h>
#include <quantiser/QuantisedFrame.h>
#include <splitter/FrameSplitter.h>
#include <splitter/WaterMolecule.h>
#include <graph/anngraphcreator/ANNGraphCreator.h>
#include <graph/gridgraphcreator/GridGraphCreator.h>

#include "Renderer.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 0.9

#define MAX_LINE_WIDTH 10

ClusterView::ClusterView()
{
    settings = new QSettings;
    viewName = "Water cluster view";
    _lineColor[0] = settings->value("ClusterView/colorR", 0.0).toDouble();
    _lineColor[1] = settings->value("ClusterView/colorG", 0.0).toDouble();
    _lineColor[2] = settings->value("ClusterView/colorB", 1.0).toDouble();
    _lineColor[3] = settings->value("ClusterView/colorA", 0.02).toDouble();
    _lineWidth = settings->value("ClusterView/lineWidth", 2).toInt();
    _preferenceWidget = NULL;

    quantised = 0;
    num_clusters = 0;
    current_cluster = -1;
}//constructor

ClusterView::~ClusterView()
{
    settings->sync();
    delete settings;
}//destructor


void ClusterView::init(std::vector<AtomInformation> pdb)
{
    this->pdb = pdb;
    FrameSplitter::split_frame(pdb, waters, others);
}//init


void ClusterView::updatePreferences()
{
    lineWidthSlider->setValue(_lineWidth);
    lineAlphaSlider->setValue((int)(_lineColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
}//updatePreferences

QWidget* ClusterView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
        setupPreferenceWidget();
    return _preferenceWidget;
}//preferenceWidget


void ClusterView::dfs(int current, int component)
{
    components[current] = component;
    sizes[component] += 1;
    for(int i = 0; i < graph[current].size(); ++i)
        if(components.find(graph[current][i]) == components.end())
            dfs(graph[current][i], component);
}//dfs

void ClusterView::tick(Frame* frame, QuantisedFrame* quantised)
{
    this->frame = frame;
    this->quantised = quantised;

    graph.clear();
    graph = ANNGraphCreator::create_graph(waters, *frame);
    // graph = GridGraphCreator::create_graph(waters, *frame);

    num_clusters = 0;
    components.clear();
    sizes.clear();
    for(int i = 0; i < waters.size(); ++i)
    {
        if(components.find(waters[i].OH2_index) == components.end())
        {
            dfs(waters[i].OH2_index, num_clusters++);
        }
    }
    clusterSpinBox->setMaximum(num_clusters-1);
    printf("Cluster count: %d\n", num_clusters-1);
}//tick

void ClusterView::render()
{
    if (quantised == NULL)
        return;

    if (parent)
        glTranslatef(-parent->volume_middle[0], -parent->volume_middle[1], -parent->volume_middle[2]);
    glColor4fv(_lineColor);
    glBegin(GL_LINES);
    int start;
    for (std::map<unsigned int, std::vector<unsigned int> >::iterator it = graph.begin(); it != graph.end(); it++)
    {
        start = it->first;
        if ((current_cluster >= 0) && (components[start] != current_cluster))
            continue;
        for (std::vector<unsigned int>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
        {
            glVertex3i(quantised->quantised_frame[3*start],
                       quantised->quantised_frame[3*start+1],
                       quantised->quantised_frame[3*start+2]);
            glVertex3i(quantised->quantised_frame[3*(*vit)],
                       quantised->quantised_frame[3*(*vit)+1],
                       quantised->quantised_frame[3*(*vit)+2]);
        }//for
    }//for
    glEnd();
}//render


void ClusterView::initGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_LIGHTING);

    glLineWidth(_lineWidth);
}//initGL


void ClusterView::setLineAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _lineColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;

    settings->setValue("ClusterView/colorA", _lineColor[3]);
}//setLineAlpha

void ClusterView::setLineWidth(int value)
{
    if (value > MAX_LINE_WIDTH) value = MAX_LINE_WIDTH;
    if (value < 1) value = 1;
    _lineWidth = value;
    if (current) glLineWidth(_lineWidth);

    settings->setValue("ClusterView/lineWidth", _lineWidth);
}//setLineWidth

void ClusterView::pickLineColor()
{
    pickColor(_lineColor);

    settings->setValue("ClusterView/colorR", _lineColor[0]);
    settings->setValue("ClusterView/colorG", _lineColor[1]);
    settings->setValue("ClusterView/colorB", _lineColor[2]);
}//pickLineColor

void ClusterView::setClusterID(int value)
{
    if (value < 0) value = -1;
    if (value >= num_clusters) value = num_clusters - 1;
    current_cluster = value;
    if (current_cluster > -1)
    printf("Cluster size: %u\n", sizes[current_cluster]);
}//setClusterID


void ClusterView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;

    QGridLayout* layout = new QGridLayout(_preferenceWidget);

    QPushButton* lineColorButton = new QPushButton(tr("Select line colour"), _preferenceWidget);
    connect(lineColorButton, SIGNAL(clicked()), this, SLOT(pickLineColor()));
    layout->addWidget(lineColorButton, 0, 0, 1, 2);

    QLabel* lineAlphaLabel = new QLabel(tr("Line alpha"), _preferenceWidget);
    layout->addWidget(lineAlphaLabel, 1, 0);

    lineAlphaSlider = new QSlider(_preferenceWidget);
    lineAlphaSlider->setOrientation(Qt::Horizontal);
    lineAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(lineAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineAlpha(int)));
    layout->addWidget(lineAlphaSlider, 1, 1);

    QLabel* lineWidthLabel = new QLabel(tr("Line width"), _preferenceWidget);
    layout->addWidget(lineWidthLabel, 2, 0);

    lineWidthSlider = new QSlider(_preferenceWidget);
    lineWidthSlider->setOrientation(Qt::Horizontal);
    lineWidthSlider->setRange(0, MAX_LINE_WIDTH);
    connect(lineWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineWidth(int)));
    layout->addWidget(lineWidthSlider, 2, 1);

    QLabel* clusterLabel = new QLabel(tr("Cluster ID"), _preferenceWidget);
    layout->addWidget(clusterLabel, 3, 0);

    clusterSpinBox = new QSpinBox(_preferenceWidget);
    clusterSpinBox->setRange(-1, -1);
    connect(clusterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setClusterID(int)));
    layout->addWidget(clusterSpinBox, 3, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

