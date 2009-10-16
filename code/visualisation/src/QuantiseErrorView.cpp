#include "QuantiseErrorView.h"

#include <cmath>
#include <cstdio>
#include <GL/gl.h>

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QWidget>

#include <quantiser/QuantisedFrame.h>

#include "Util.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 1.0

#define MAX_LINE_WIDTH 10


QuantiseErrorView::QuantiseErrorView()
{
    settings = new QSettings;
    viewName = "Quantisation error view";
    _startColor[0] = settings->value("QuantiseErrorView/startColorR", 0.2).toDouble();
    _startColor[1] = settings->value("QuantiseErrorView/startColorG", 0.2).toDouble();
    _startColor[2] = settings->value("QuantiseErrorView/startColorB", 0.2).toDouble();
    _startColor[3] = settings->value("QuantiseErrorView/startColorA", 0.5).toDouble();

    _finalColor[0] = settings->value("QuantiseErrorView/finalColorR", 1.0).toDouble();
    _finalColor[1] = settings->value("QuantiseErrorView/finalColorG", 0.0).toDouble();
    _finalColor[2] = settings->value("QuantiseErrorView/finalColorB", 0.0).toDouble();
    _finalColor[3] = settings->value("QuantiseErrorView/finalColorA", 1.0).toDouble();
    _preferenceWidget = NULL;
    _lineSize = settings->value("QuantiseErrorView/lineSize", 2).toInt();
    _startErrorValue = settings->value("QuantiseErrorView/startError", 0.06).toDouble();
    _finalErrorValue = settings->value("QuantiseErrorView/finalError", 0.06).toDouble();
    errors = 0;
    doSplitWaters = true;
}//constructor

QuantiseErrorView::~QuantiseErrorView()
{
    settings->sync();
    delete settings;
}//destructor


void QuantiseErrorView::updatePreferences()
{
    lineSizeSlider->setValue(_lineSize);
    startAlphaSlider->setValue((int)(_startColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    finalAlphaSlider->setValue((int)(_finalColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    startErrorSpinBox->setValue(_startErrorValue);
    finalErrorSpinBox->setValue(_finalErrorValue);
}//updatePreferences

void QuantiseErrorView::setupPreferenceWidget(QWidget* preferenceWidget)
{
    QGridLayout* layout = new QGridLayout(preferenceWidget);

    QLabel* startErrorLabel = new QLabel(tr("Start error level"), preferenceWidget);
    layout->addWidget(startErrorLabel, 0, 0);

    startErrorSpinBox = new QDoubleSpinBox(preferenceWidget);
    startErrorSpinBox->setRange(0, 10.0);
    startErrorSpinBox->setDecimals(3);
    startErrorSpinBox->setSingleStep(0.01);
    connect(startErrorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setStartErrorValue(double)));
    layout->addWidget(startErrorSpinBox, 0, 1);

    QPushButton* startColorButton = new QPushButton(tr("Select start error colour"), preferenceWidget);
    connect(startColorButton, SIGNAL(clicked()), this, SLOT(pickStartColor()));
    layout->addWidget(startColorButton, 1, 0, 1, 2);

    QLabel* startAlphaLabel = new QLabel(tr("Start error alpha"), preferenceWidget);
    layout->addWidget(startAlphaLabel, 2, 0);

    startAlphaSlider = new QSlider(preferenceWidget);
    startAlphaSlider->setOrientation(Qt::Horizontal);
    startAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(startAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setStartAlpha(int)));
    layout->addWidget(startAlphaSlider, 2, 1);

    QLabel* finalErrorLabel = new QLabel(tr("Final error level"), preferenceWidget);
    layout->addWidget(finalErrorLabel, 3, 0);

    finalErrorSpinBox = new QDoubleSpinBox(preferenceWidget);
    finalErrorSpinBox->setRange(0, 10.0);
    finalErrorSpinBox->setDecimals(3);
    finalErrorSpinBox->setSingleStep(0.01);
    connect(finalErrorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFinalErrorValue(double)));
    layout->addWidget(finalErrorSpinBox, 3, 1);

    QPushButton* finalColorButton = new QPushButton(tr("Select end error colour"), preferenceWidget);
    connect(finalColorButton, SIGNAL(clicked()), this, SLOT(pickFinalColor()));
    layout->addWidget(finalColorButton, 4, 0, 1, 2);

    QLabel* finalAlphaLabel = new QLabel(tr("End error alpha"), preferenceWidget);
    layout->addWidget(finalAlphaLabel, 5, 0);

    finalAlphaSlider = new QSlider(preferenceWidget);
    finalAlphaSlider->setOrientation(Qt::Horizontal);
    finalAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(finalAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setFinalAlpha(int)));
    layout->addWidget(finalAlphaSlider, 5, 1);

    QLabel* lineSizeLabel = new QLabel(tr("Line width"), preferenceWidget);
    layout->addWidget(lineSizeLabel, 6, 0);

    lineSizeSlider = new QSlider(preferenceWidget);
    lineSizeSlider->setOrientation(Qt::Horizontal);
    lineSizeSlider->setRange(0, MAX_LINE_WIDTH);
    connect(lineSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineSize(int)));
    layout->addWidget(lineSizeSlider, 6, 1);

    preferenceWidget->setLayout(layout);
}//setupPreferenceWidget


void QuantiseErrorView::initGL()
{
    glLineWidth(_lineSize);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_LIGHTING);
}//initGL

void QuantiseErrorView::tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised)
{
    BaseView::tick(framenum, unquantised, quantised, dequantised);
    if (dequantised == NULL) return;

    float dif[3];
    float d;
    float range = _finalErrorValue - _startErrorValue;
    if (errors) delete errors;
    errors = new float[waters.size()];
    for (int i = 0; i < waters.size(); i++)
    {
        dif[0] = dequantised->atom_data[3*waters[i].OH2_index  ] - unquantised->atom_data[3*waters[i].OH2_index];
        dif[1] = dequantised->atom_data[3*waters[i].OH2_index+1] - unquantised->atom_data[3*waters[i].OH2_index+1];
        dif[2] = dequantised->atom_data[3*waters[i].OH2_index+2] - unquantised->atom_data[3*waters[i].OH2_index+2];
        d = sqrt(len2(dif));
        errors[i] = (d - _startErrorValue) / range;
        if (errors[i] < 0) errors[i] = 0;
        if (errors[i] > 1) errors[i] = 1;
        if (errors[i] > 0.5) errors[i] = 1;
    }//for
}//tick

void QuantiseErrorView::render()
{
    if (dequantised == NULL) return;

    //draw points
    float range[] = {_finalColor[0] - _startColor[0],
                     _finalColor[1] - _startColor[1],
                     _finalColor[2] - _startColor[2],
                     _finalColor[3] - _startColor[3]};
    glBegin(GL_LINES);
    for (int i = 0; i < waters.size(); i++)
    {
        if (errors[i] > 1e-6)
        {
            glColor4f(_startColor[0] + errors[i] * range[0],
                      _startColor[1] + errors[i] * range[1],
                      _startColor[2] + errors[i] * range[2],
                      _startColor[3] + errors[i] * range[3]);
            glVertex3f(unquantised->atom_data[3*waters[i].OH2_index],
                       unquantised->atom_data[3*waters[i].OH2_index+1],
                       unquantised->atom_data[3*waters[i].OH2_index+2]);
            glVertex3f(dequantised->atom_data[3*waters[i].OH2_index],
                       dequantised->atom_data[3*waters[i].OH2_index+1],
                       dequantised->atom_data[3*waters[i].OH2_index+2]);
        }//if
    }//for
    glEnd();
}//render


void QuantiseErrorView::setLineSize(int value)
{
    if (value > MAX_LINE_WIDTH) value = MAX_LINE_WIDTH;
    if (value < 1) value = 1;
    _lineSize = value;
    if (current) glLineWidth(_lineSize);

    settings->setValue("QuantiseErrorView/lineSize", _lineSize);
}//setLineSize

void QuantiseErrorView::setStartErrorValue(double value)
{
    _startErrorValue = value;

    settings->setValue("QuantiseErrorView/startError", _startErrorValue);
    tick(framenum, unquantised, quantised, dequantised);
}//setStartErrorValue

void QuantiseErrorView::setFinalErrorValue(double value)
{
    _finalErrorValue = value;

    settings->setValue("QuantiseErrorView/finalError", _finalErrorValue);
    tick(framenum, unquantised, quantised, dequantised);
}//setFinalErrorValue

void QuantiseErrorView::pickStartColor()
{
    pickColor(_startColor);

    settings->setValue("QuantiseErrorView/startColorR", _startColor[0]);
    settings->setValue("QuantiseErrorView/startColorG", _startColor[1]);
    settings->setValue("QuantiseErrorView/startColorB", _startColor[2]);
}//pickStartColor

void QuantiseErrorView::pickFinalColor()
{
    pickColor(_finalColor);

    settings->setValue("QuantiseErrorView/finalColorR", _finalColor[0]);
    settings->setValue("QuantiseErrorView/finalColorG", _finalColor[1]);
    settings->setValue("QuantiseErrorView/finalColorB", _finalColor[2]);
}//pickFinalColor

void QuantiseErrorView::setStartAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _startColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;

    settings->setValue("QuantiseErrorView/startColorA", _startColor[3]);
}//setStartAlpha

void QuantiseErrorView::setFinalAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _finalColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;

    settings->setValue("QuantiseErrorView/finalColorA", _finalColor[3]);
}//setFinalAlpha

