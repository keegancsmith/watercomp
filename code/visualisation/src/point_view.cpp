#include "point_view.h"

#include <cstdio>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include <GL/gl.h>

#include "frame_data.h"

#define ALPHA_MAX_SLIDER 100
#define ALPHA_MAX_VAL 0.1

PointView::PointView()
{
    viewName = "Point data view";
    _pointColor[0] = 0.0f;
    _pointColor[1] = 0.0f;
    _pointColor[2] = 1.0f;
    _pointColor[3] = 0.02f;
    data = 0;
    _preferenceWidget = NULL;
}//constructor

PointView::~PointView()
{
}//destructor


void PointView::updatePreferences()
{
    pointAlphaSlider->setValue((int)(_pointColor[3] * ALPHA_MAX_SLIDER / ALPHA_MAX_VAL));
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
    //draw points
    glColor4fv(_pointColor);
    glBegin(GL_POINTS);
    for (int i = 0; i < data->natoms(); i++)
    {
        glVertex3dv(data->at(i).pos);
    }//for
    glEnd();
}//render


void PointView::setPointAlpha(int value)
{
    if (value > ALPHA_MAX_SLIDER) value = ALPHA_MAX_SLIDER;
    if (value < 0) value = 0;
    _pointColor[3] = (float)value * ALPHA_MAX_VAL / ALPHA_MAX_SLIDER;
}//setPointAlpha

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

    pointAlphaLabel = new QLabel(tr("Point alpha"), _preferenceWidget);
    layout->addWidget(pointAlphaLabel, 1, 0);

    pointAlphaSlider = new QSlider(_preferenceWidget);
    pointAlphaSlider->setOrientation(Qt::Horizontal);
    pointAlphaSlider->setRange(0, ALPHA_MAX_SLIDER);
    connect(pointAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointAlpha(int)));
    layout->addWidget(pointAlphaSlider, 1, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

