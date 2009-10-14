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
#include "Util.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 0.9

#define MAX_LINE_WIDTH 10




void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric)
{
    float vx = x2-x1;
    float vy = y2-y1;
    float vz = z2-z1;

    //handle the degenerate case of z1 == z2 with an approximation
    if(vz == 0)
        vz = 1;
        // vz = .00000001;

    float v = sqrt( vx*vx + vy*vy + vz*vz );
    // float ax = 57.2957795*acos( vz/v );
    float ax = DEG(acos( vz/v ));
    if ( vz < 0.0 )
        ax = -ax;
    float rx = -vy*vz;
    float ry = vx*vz;
    glPushMatrix();

    //draw the cylinder body
    glTranslatef( x1,y1,z1 );
    glRotatef(ax, rx, ry, 0.0);
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluCylinder(quadric, radius, radius, v, subdivisions, 1);

    //draw the first cap
    gluQuadricOrientation(quadric,GLU_INSIDE);
    gluDisk( quadric, 0.0, radius, subdivisions, 1);
    glTranslatef( 0,0,v );

    //draw the second cap
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluDisk( quadric, 0.0, radius, subdivisions, 1);
    glPopMatrix();
}



ClusterView::ClusterView()
{
    settings = new QSettings;
    viewName = "Water cluster view";
    _lineColor[0] = settings->value("ClusterView/colorR", 0.0).toDouble();
    _lineColor[1] = settings->value("ClusterView/colorG", 0.0).toDouble();
    _lineColor[2] = settings->value("ClusterView/colorB", 1.0).toDouble();
    _lineColor[3] = settings->value("ClusterView/colorA", 0.8).toDouble();
    _lineWidth = settings->value("ClusterView/lineWidth", 2).toInt();
    _preferenceWidget = NULL;

    quantised = 0;
    num_clusters = 0;
    current_cluster = -1;

    quadric = gluNewQuadric();

    first = true;
}//constructor

ClusterView::~ClusterView()
{
    settings->sync();
    delete settings;
    gluDeleteQuadric(quadric);
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

void ClusterView::tick(int framenum, Frame* frame, QuantisedFrame* quantised)
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
            sizes[num_clusters] = 0;
            dfs(waters[i].OH2_index, num_clusters++);
        }
    }
    clusterSpinBox->setMaximum(num_clusters-1);
    printf("Cluster count: %d\n", num_clusters-1);
    if (current_cluster == -1) countLabel->setNum((int)(num_clusters-1));
}//tick

// #define DRAW_LINES

void ClusterView::render()
{
    if (quantised == NULL)
        return;

    double radius = 2;
    float dif[3];
    double height = 2;
    int hslice = 16;
    int vslice = 4;

    if (parent)
        glTranslatef(-parent->volume_middle[0], -parent->volume_middle[1], -parent->volume_middle[2]);

    float vec[3];
    int count = 0;
    int a, v, ax, rx, ry;

    glColor4fv(_lineColor);
#ifdef DRAW_LINES
    glBegin(GL_LINES);
#endif
    int start;
    for (std::map<unsigned int, std::vector<unsigned int> >::iterator it = graph.begin(); it != graph.end(); it++)
    {
        start = it->first;
        if ((current_cluster >= 0) && (components[start] != current_cluster))
            continue;
        if (count++ > 20)
            break;
        for (std::vector<unsigned int>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
        {

// #ifdef DRAW_LINES
            glBegin(GL_LINES);
            glVertex3i(quantised->quantised_frame[3*start],
                       quantised->quantised_frame[3*start+1],
                       quantised->quantised_frame[3*start+2]);
            glVertex3i(quantised->quantised_frame[3*(*vit)],
                       quantised->quantised_frame[3*(*vit)+1],
                       quantised->quantised_frame[3*(*vit)+2]);
            glEnd();
            if (first)
            {
                printf("%u %u %u ~ %u %u %u\n",
                        quantised->quantised_frame[3*start],
                        quantised->quantised_frame[3*start+1],
                        quantised->quantised_frame[3*start+2],
                        quantised->quantised_frame[3*(*vit)],
                        quantised->quantised_frame[3*(*vit)+1],
                        quantised->quantised_frame[3*(*vit)+2]);
            }//if
// #else

            renderCylinder(quantised->quantised_frame[3*start],
                           quantised->quantised_frame[3*start+1],
                           quantised->quantised_frame[3*start+2],
                           quantised->quantised_frame[3*(*vit)],
                           quantised->quantised_frame[3*(*vit)+1],
                           quantised->quantised_frame[3*(*vit)+2],
                           radius, 8, quadric);

            /*
            for (a = 0; a < 3; a++)
                vec[a] = quantised->quantised_frame[3*(*vit)+a]
                         - quantised->quantised_frame[3*start+a];
            v = sqrt(len2(vec));
            if (v < EPSILON)
                continue;

            ax = DEG(acos(vec[2]/v));
            if (vec[2] < 0)
                ax = -ax;
            rx = -vec[1]*vec[2];
            ry = vec[0]*vec[2];



            glPushMatrix();

            // draw the body
            glTranslatef(quantised->quantised_frame[3*start],
                       quantised->quantised_frame[3*start+1],
                       quantised->quantised_frame[3*start+2]);
            glRotatef(ax, rx, ry, 0.0);
            gluQuadricOrientation(quadric,GLU_OUTSIDE);
            gluCylinder(quadric, radius, radius, v, hslice, 1);

            // draw the first cap
            gluQuadricOrientation(quadric,GLU_INSIDE);
            gluDisk(quadric, 0.0, radius, hslice, 1);

            // draw the second cap
            glTranslatef(0, 0, v);
            gluQuadricOrientation(quadric,GLU_OUTSIDE);
            gluDisk(quadric, 0.0, radius, hslice, 1);

            glPopMatrix();
            */


// #endif

        }//for
    }//for
#ifdef DRAW_LINES
    glEnd();
#endif
    first = false;
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
    // countLabel->setNum((int)(current_cluster > -1 ? sizes[current_cluster] : num_clusters-1));
    countLabel->setNum((int)(current_cluster > -1 ? sizes[current_cluster] : num_clusters-1));
    first = true;
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

    QLabel* countLabelLabel = new QLabel(tr("Cluster count"), _preferenceWidget);
    layout->addWidget(countLabelLabel, 4, 0);

    countLabel = new QLabel(tr("-1"), _preferenceWidget);
    layout->addWidget(countLabel, 4, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

