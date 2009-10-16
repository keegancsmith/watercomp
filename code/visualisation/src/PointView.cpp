#include "PointView.h"

#include <cstdio>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QWidget>
#include <GL/gl.h>

#include <quantiser/QuantisedFrame.h>

#include "Renderer.h"

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
    quantised = 0;
    _preferenceWidget = NULL;
    _pointSize = settings->value("PointView/pointSize", 10).toInt();
    doSplitWaters = true;
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

void PointView::setupPreferenceWidget(QWidget* preferenceWidget)
{
    QGridLayout* layout = new QGridLayout(preferenceWidget);

    QPushButton* pointColorButton = new QPushButton(tr("Select point colour"), preferenceWidget);
    connect(pointColorButton, SIGNAL(clicked()), this, SLOT(pickPointColor()));
    layout->addWidget(pointColorButton, 0, 0, 1, 2);

    QLabel* pointSizeLabel = new QLabel(tr("Point size"), preferenceWidget);
    layout->addWidget(pointSizeLabel, 1, 0);

    pointSizeSlider = new QSlider(preferenceWidget);
    pointSizeSlider->setOrientation(Qt::Horizontal);
    pointSizeSlider->setRange(0, MAX_POINT_SIZE);
    connect(pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointSize(int)));
    layout->addWidget(pointSizeSlider, 1, 1);

    QLabel* pointAlphaLabel = new QLabel(tr("Point alpha"), preferenceWidget);
    layout->addWidget(pointAlphaLabel, 2, 0);

    pointAlphaSlider = new QSlider(preferenceWidget);
    pointAlphaSlider->setOrientation(Qt::Horizontal);
    pointAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(pointAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointAlpha(int)));
    layout->addWidget(pointAlphaSlider, 2, 1);

    preferenceWidget->setLayout(layout);
}//setupPreferenceWidget


void PointView::initGL()
{
    glPointSize(_pointSize);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_LIGHTING);
}//initGL

void PointView::render()
{
    if (dequantised == NULL) return;

    glDepthFunc(GL_ALWAYS);
    //draw points
    glColor4fv(_pointColor);
    glBegin(GL_POINTS);
    for (int i = 0; i < waters.size(); i++)
    {
        glVertex3i(dequantised->atom_data[3*waters[i].OH2_index],
                   dequantised->atom_data[3*waters[i].OH2_index+1],
                   dequantised->atom_data[3*waters[i].OH2_index+2]);
    }//for
    glEnd();
    glDepthFunc(GL_LEQUAL);
}//render


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

