#include "point_view.h"

#include <cstdio>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QWidget>
#include <GL/gl.h>

#include <quantiser/QuantisedFrame.h>

#include "renderer.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 0.1

#define MAX_POINT_SIZE 20

PointView::PointView()
{
    settings = new QSettings;
    viewName = "Point data view";
    _pointColor[0] = settings->value("PointView/colorR", 0.0).toDouble();
    _pointColor[1] = settings->value("PointView/colorG", 0.0).toDouble();
    _pointColor[2] = settings->value("PointView/colorB", 1.0).toDouble();
    _pointColor[3] = settings->value("PointView/colorA", 0.02).toDouble();
    data = 0;
    _preferenceWidget = NULL;
    _pointSize = settings->value("PointView/pointSize", 10).toInt();
}//constructor

PointView::~PointView()
{
    settings->sync();
    delete settings;
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


void PointView::tick(Frame* frame, QuantisedFrame* data)
{
    this->frame = frame;
    this->data = data;
}//tick

void PointView::render()
{
    if (data == NULL)
        return;
    if (parent)
        glTranslatef(-parent->volume_middle[0], -parent->volume_middle[1], -parent->volume_middle[2]);
    glDepthFunc(GL_ALWAYS);
    //draw points
    glColor4fv(_pointColor);
    glBegin(GL_POINTS);
    for (int i = 0; i < data->natoms(); i++)
    {
        if (pdb[i].atom_name == "OH2")
            glVertex3i(data->quantised_frame[i*3],
                    data->quantised_frame[i*3+1],
                    data->quantised_frame[i*3+2]);
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

    settings->setValue("PointView/colorA", _pointColor[3]);
}//setPointAlpha

void PointView::setPointSize(int value)
{
    if (value > MAX_POINT_SIZE) value = MAX_POINT_SIZE;
    if (value < 1) value = 1;
    _pointSize = value;
    if (current) glPointSize(_pointSize);

    settings->setValue("PointView/pointSize", _pointSize);
}//setPointSize

void PointView::pickPointColor()
{
    pickColor(_pointColor);

    settings->setValue("PointView/colorR", _pointColor[0]);
    settings->setValue("PointView/colorG", _pointColor[1]);
    settings->setValue("PointView/colorB", _pointColor[2]);
}//pickPointColor


void PointView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;

    QGridLayout* layout = new QGridLayout(_preferenceWidget);

    QPushButton* pointColorButton = new QPushButton(tr("Select point colour"), _preferenceWidget);
    connect(pointColorButton, SIGNAL(clicked()), this, SLOT(pickPointColor()));
    layout->addWidget(pointColorButton, 0, 0, 1, 2);

    QLabel* pointSizeLabel = new QLabel(tr("Point size"), _preferenceWidget);
    layout->addWidget(pointSizeLabel, 1, 0);

    pointSizeSlider = new QSlider(_preferenceWidget);
    pointSizeSlider->setOrientation(Qt::Horizontal);
    pointSizeSlider->setRange(0, MAX_POINT_SIZE);
    connect(pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointSize(int)));
    layout->addWidget(pointSizeSlider, 1, 1);

    QLabel* pointAlphaLabel = new QLabel(tr("Point alpha"), _preferenceWidget);
    layout->addWidget(pointAlphaLabel, 2, 0);

    pointAlphaSlider = new QSlider(_preferenceWidget);
    pointAlphaSlider->setOrientation(Qt::Horizontal);
    pointAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(pointAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointAlpha(int)));
    layout->addWidget(pointAlphaSlider, 2, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

