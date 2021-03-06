#include "ClusterView.h"

#include <cmath>
#include <cstdio>
#include <map>
#include <vector>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <GL/gl.h>

#include <quantiser/QuantisedFrame.h>
#include <graph/anngraphcreator/ANNGraphCreator.h>

#include "Renderer.h"
#include "Util.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 1.0

#define MAX_LINE_WIDTH 10



ClusterView::ClusterView()
{
    settings = new QSettings;
    viewName = "Water cluster view";
    _lineColor[0] = settings->value("ClusterView/colorR", 0.0).toDouble();
    _lineColor[1] = settings->value("ClusterView/colorG", 0.0).toDouble();
    _lineColor[2] = settings->value("ClusterView/colorB", 1.0).toDouble();
    _lineColor[3] = settings->value("ClusterView/colorA", 0.8).toDouble();
    _lineWidth = settings->value("ClusterView/lineWidth", 2).toInt();

    lighting = settings->value("ClusterView/lighting", false).toBool();
    tolerance = settings->value("ClusterView/tolerance", 0.2).toDouble();
    totalToDraw = settings->value("ClusterView/draw", 20).toInt();
    _preferenceWidget = NULL;

    quantised = 0;
    num_clusters = 0;
    current_cluster = -1;

    quadric = gluNewQuadric();

    first = true;
    doSplitWaters = true;
}//constructor

ClusterView::~ClusterView()
{
    settings->sync();
    delete settings;
    gluDeleteQuadric(quadric);
}//destructor


void ClusterView::updatePreferences()
{
    lineWidthSlider->setValue(_lineWidth);
    lineAlphaSlider->setValue((int)(_lineColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    lightCheckBox->setCheckState(lighting ? Qt::Checked : Qt::Unchecked);
    clusterSpinBox->setMaximum(num_clusters-1);
    if (current_cluster == -1) countLabel->setNum((int)(num_clusters-1));
    toleranceSpinBox->setValue(tolerance);
    totalSpinBox->setValue(totalToDraw);
    if (totalToDraw <= num_clusters)
        totalSpinBox->setMaximum(num_clusters-1);
}//updatePreferences

void ClusterView::setupPreferenceWidget(QWidget* preferenceWidget)
{
    QGridLayout* layout = new QGridLayout(preferenceWidget);

    QPushButton* lineColorButton = new QPushButton(tr("Select line colour"), preferenceWidget);
    connect(lineColorButton, SIGNAL(clicked()), this, SLOT(pickLineColor()));
    layout->addWidget(lineColorButton, 0, 0, 1, 2);

    QLabel* lineAlphaLabel = new QLabel(tr("Line alpha"), preferenceWidget);
    layout->addWidget(lineAlphaLabel, 1, 0);

    lineAlphaSlider = new QSlider(preferenceWidget);
    lineAlphaSlider->setOrientation(Qt::Horizontal);
    lineAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(lineAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineAlpha(int)));
    layout->addWidget(lineAlphaSlider, 1, 1);

    QLabel* lineWidthLabel = new QLabel(tr("Line width"), preferenceWidget);
    layout->addWidget(lineWidthLabel, 2, 0);

    lineWidthSlider = new QSlider(preferenceWidget);
    lineWidthSlider->setOrientation(Qt::Horizontal);
    lineWidthSlider->setRange(0, MAX_LINE_WIDTH);
    connect(lineWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineWidth(int)));
    layout->addWidget(lineWidthSlider, 2, 1);

    QLabel* lightLabel = new QLabel(tr("Enable lighting"), preferenceWidget);
    layout->addWidget(lightLabel, 3, 0);

    lightCheckBox = new QCheckBox(preferenceWidget);
    connect(lightCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLighting(int)));
    layout->addWidget(lightCheckBox, 3, 1);

    QLabel* toleranceLabel = new QLabel(tr("Tolerance"), preferenceWidget);
    layout->addWidget(toleranceLabel, 4, 0);

    toleranceSpinBox = new QDoubleSpinBox(preferenceWidget);
    toleranceSpinBox->setRange(0.1, 6.0);
    toleranceSpinBox->setDecimals(1);
    toleranceSpinBox->setSingleStep(0.1);
    connect(toleranceSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setTolerance(double)));
    layout->addWidget(toleranceSpinBox, 4, 1);

    QLabel* totalLabel = new QLabel(tr("Clusters drawn"), preferenceWidget);
    layout->addWidget(totalLabel, 5, 0);

    totalSpinBox = new QSpinBox(preferenceWidget);
    totalSpinBox->setMinimum(1);
    connect(totalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTotal(int)));
    layout->addWidget(totalSpinBox, 5, 1);

    QLabel* clusterLabel = new QLabel(tr("Cluster ID"), preferenceWidget);
    layout->addWidget(clusterLabel, 6, 0);

    clusterSpinBox = new QSpinBox(preferenceWidget);
    clusterSpinBox->setRange(-1, -1);
    connect(clusterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setClusterID(int)));
    layout->addWidget(clusterSpinBox, 6, 1);

    QLabel* countLabelLabel = new QLabel(tr("Cluster count"), preferenceWidget);
    layout->addWidget(countLabelLabel, 7, 0);

    countLabel = new QLabel(tr("-1"), preferenceWidget);
    layout->addWidget(countLabel, 7, 1);

    preferenceWidget->setLayout(layout);
}//setupPreferenceWidget


void ClusterView::dfs(int current, int component)
{
    components[current] = component;
    sizes[component] += 1;
    for(int i = 0; i < graph[current].size(); ++i)
        if(components.find(graph[current][i]) == components.end())
            dfs(graph[current][i], component);
}//dfs

void ClusterView::tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised)
{
    BaseView::tick(framenum, unquantised, quantised, dequantised);
    if (unquantised == NULL) return;

    graph.clear();
    graph = create_graph(waters, *unquantised, tolerance);

    num_clusters = 0;
    components.clear();
    sizes.clear();
    for (int i = 0; i < waters.size(); ++i)
    {
        if (components.find(waters[i].OH2_index) == components.end())
        {
            sizes[num_clusters] = 0;
            dfs(waters[i].OH2_index, num_clusters++);
        }//if
    }//for
}//tick

void ClusterView::initGL()
{
    glLineWidth(_lineWidth);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float l0_amb[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float l0_dif[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float l0_spe[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);

    float l1_amb[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float l1_dif[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float l1_spe[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, l1_amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, l1_spe);

    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    if (lighting) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
}//initGL

#define DRAW_LINES

void ClusterView::render()
{
    Frame* data = drawQuantised ? dequantised : unquantised;
    if (data == NULL) return;

    float l0_pos[] = {-1.0f, 1.0f, 2.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

    float l1_pos[] = {0.0f, 0.0f, -1.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);

    double radius = 2;
    int count = 0;
    int start;
    glColor4fv(_lineColor);
    for (std::map<unsigned int, std::vector<unsigned int> >::iterator it = graph.begin(); it != graph.end(); it++)
    {
        start = it->first;
        if ((current_cluster >= 0) && (components[start] != current_cluster))
            continue;
        if (count++ > totalToDraw)
            break;
        for (std::vector<unsigned int>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
        {

#ifdef DRAW_LINES
            glBegin(GL_LINES);
            glVertex3i(data->atom_data[3*start],
                       data->atom_data[3*start+1],
                       data->atom_data[3*start+2]);
            glVertex3i(data->atom_data[3*(*vit)],
                       data->atom_data[3*(*vit)+1],
                       data->atom_data[3*(*vit)+2]);
            glEnd();
#endif
            // if (first)
            // {
                // printf("%f %f %f ~ %f %f %f\n",
                        // dequantised->atom_data[3*start],
                        // dequantised->atom_data[3*start+1],
                        // dequantised->atom_data[3*start+2],
                        // dequantised->atom_data[3*(*vit)],
                        // dequantised->atom_data[3*(*vit)+1],
                        // dequantised->atom_data[3*(*vit)+2]);
            // }//if

            renderCylinder(data->atom_data[3*start],
                           data->atom_data[3*start+1],
                           data->atom_data[3*start+2],
                           data->atom_data[3*(*vit)],
                           data->atom_data[3*(*vit)+1],
                           data->atom_data[3*(*vit)+2],
                           radius, 8, quadric);
        }//for
    }//for
    first = false;
}//render


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
    countLabel->setNum((int)(current_cluster > -1 ? sizes[current_cluster] : num_clusters-1));
    first = true;
}//setClusterID

void ClusterView::setLighting(int state)
{
    lighting = state != 0;
    settings->setValue("ClusterView/lighting", lighting);

    if (current)
    {
        if (lighting) glEnable(GL_LIGHTING);
        else glDisable(GL_LIGHTING);
    }//if
}//setLighting

void ClusterView::setTolerance(double value)
{
    tolerance = value;
    settings->setValue("ClusterView/tolerance", tolerance);
    tick(framenum, unquantised, quantised, dequantised);
}//setTolerance

void ClusterView::setTotal(int value)
{
    totalToDraw = value;
    settings->setValue("ClusterView/draw", totalToDraw);
}//setTotal

