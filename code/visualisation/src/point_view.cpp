#include "point_view.h"

#include <cstdio>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include <GL/gl.h>

#include "frame_data.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 0.1

#define MAX_POINT_SIZE 100

PointView::PointView()
{
    viewName = "Point data view";
    _pointColor[0] = 0.0f;
    _pointColor[1] = 0.0f;
    _pointColor[2] = 1.0f;
    _pointColor[3] = 0.02f;
    data = 0;
    _preferenceWidget = NULL;
    _pointSize = 10;
}//constructor

PointView::~PointView()
{
}//destructor


void PointView::updatePreferences()
{
    pointSizeSlider->setValue(_pointSize);
    pointAlphaSlider->setValue((int)(_pointColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
}//updatePreferences

QWidget* PointView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
        setupPreferenceWidget();
    return _preferenceWidget;
}//preferenceWidget


void PointView::tick(Frame_Data* data)
{
    this->data = data;
}//tick

void PointView::render()
{
    glDepthFunc(GL_ALWAYS);
    //draw points
    glColor4fv(_pointColor);
    glBegin(GL_POINTS);
    for (int i = 0; i < data->natoms(); i++)
    {
        glVertex3dv(data->at(i).pos);
    }//for
    glEnd();
    glDepthFunc(GL_LEQUAL);
}//render


void PointView::initGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(_pointSize);

    glDisable(GL_LIGHTING);
}//initGL


void PointView::setPointAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _pointColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;
}//setPointAlpha

void PointView::setPointSize(int value)
{
    if (value > MAX_POINT_SIZE) value = MAX_POINT_SIZE;
    if (value < 1) value = 1;
    _pointSize = value;
    glPointSize(_pointSize);
}//setPointSize

void PointView::pickPointColor()
{
    pickColor(_pointColor);
}//pickPointColor


void PointView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;

    layout = new QGridLayout(_preferenceWidget);

    pointColorButton = new QPushButton(tr("Select point colour"), _preferenceWidget);
    connect(pointColorButton, SIGNAL(clicked()), this, SLOT(pickPointColor()));
    layout->addWidget(pointColorButton, 0, 0, 1, 2);

    pointSizeLabel = new QLabel(tr("Point size"), _preferenceWidget);
    layout->addWidget(pointSizeLabel, 1, 0);

    pointSizeSlider = new QSlider(_preferenceWidget);
    pointSizeSlider->setOrientation(Qt::Horizontal);
    pointSizeSlider->setRange(0, MAX_POINT_SIZE);
    connect(pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointSize(int)));
    layout->addWidget(pointSizeSlider, 1, 1);

    pointAlphaLabel = new QLabel(tr("Point alpha"), _preferenceWidget);
    layout->addWidget(pointAlphaLabel, 2, 0);

    pointAlphaSlider = new QSlider(_preferenceWidget);
    pointAlphaSlider->setOrientation(Qt::Horizontal);
    pointAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(pointAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointAlpha(int)));
    layout->addWidget(pointAlphaSlider, 2, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

