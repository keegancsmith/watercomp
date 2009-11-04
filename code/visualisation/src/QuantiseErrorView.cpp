#include "QuantiseErrorView.h"

#include <cmath>
#include <cstdio>
#include <GL/gl.h>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSpacerItem>
#include <QWidget>

#include <quantiser/QuantisedFrame.h>

#include "Util.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 1.0

#define MAX_POINT_SIZE 20

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
    _pointSize = settings->value("QuantiseErrorView/pointSize", 10).toInt();
    _lineSize = settings->value("QuantiseErrorView/lineSize", 2).toInt();
    _startErrorValue = settings->value("QuantiseErrorView/startError", 0.06).toDouble();
    _finalErrorValue = settings->value("QuantiseErrorView/finalError", 0.06).toDouble();
    _thresholdValue = settings->value("QuantiseErrorView/thresholdValue", 0.5).toDouble();
    _drawLine = settings->value("QuantiseErrorView/drawLine", true).toBool();
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
    pointSizeSlider->setValue(_pointSize);
    lineSizeSlider->setValue(_lineSize);
    startAlphaSlider->setValue((int)(_startColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    finalAlphaSlider->setValue((int)(_finalColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    startErrorSpinBox->setValue(_startErrorValue);
    finalErrorSpinBox->setValue(_finalErrorValue);
    thresholdSlider->setValue(_thresholdValue * 100);
    drawLineCheckBox->setCheckState(_drawLine ? Qt::Checked : Qt::Unchecked);
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

    layout->addItem(new QSpacerItem(6, 24), 7, 0);

    QLabel* drawLineLabel = new QLabel(tr("Draw line"), preferenceWidget);
    layout->addWidget(drawLineLabel, 7, 0);

    drawLineCheckBox = new QCheckBox(preferenceWidget);
    connect(drawLineCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setDrawLine(int)));
    layout->addWidget(drawLineCheckBox, 7, 1);

    QLabel* pointSizeLabel = new QLabel(tr("Point size"), preferenceWidget);
    layout->addWidget(pointSizeLabel, 8, 0);

    pointSizeSlider = new QSlider(preferenceWidget);
    pointSizeSlider->setOrientation(Qt::Horizontal);
    pointSizeSlider->setRange(0, MAX_POINT_SIZE);
    connect(pointSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setPointSize(int)));
    layout->addWidget(pointSizeSlider, 8, 1);

    QLabel* lineSizeLabel = new QLabel(tr("Line width"), preferenceWidget);
    layout->addWidget(lineSizeLabel, 9, 0);

    lineSizeSlider = new QSlider(preferenceWidget);
    lineSizeSlider->setOrientation(Qt::Horizontal);
    lineSizeSlider->setRange(0, MAX_LINE_WIDTH);
    connect(lineSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineSize(int)));
    layout->addWidget(lineSizeSlider, 9, 1);

    QLabel* thresholdLabel = new QLabel(tr("Middle threshold value"), preferenceWidget);
    layout->addWidget(thresholdLabel, 10, 0);

    thresholdSlider = new QSlider(preferenceWidget);
    thresholdSlider->setOrientation(Qt::Horizontal);
    thresholdSlider->setRange(0, 100);
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(setThresholdValue(int)));
    layout->addWidget(thresholdSlider, 10, 1);

    QLabel* maxErrorLabelLabel = new QLabel(tr("Maximum error"), preferenceWidget);
    layout->addWidget(maxErrorLabelLabel, 11, 0);

    maxErrorLabel = new QLabel(tr("0"), preferenceWidget);
    layout->addWidget(maxErrorLabel, 11, 1);

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
    float maxError = 0;
    for (int i = 0; i < waters.size(); i++)
    {
        dif[0] = dequantised->atom_data[3*waters[i].OH2_index  ] - unquantised->atom_data[3*waters[i].OH2_index];
        dif[1] = dequantised->atom_data[3*waters[i].OH2_index+1] - unquantised->atom_data[3*waters[i].OH2_index+1];
        dif[2] = dequantised->atom_data[3*waters[i].OH2_index+2] - unquantised->atom_data[3*waters[i].OH2_index+2];
        d = sqrt(len2(dif));
        if (d > maxError) maxError = d;
        errors[i] = (d - _startErrorValue) / range;
        if (errors[i] < 0) errors[i] = 0;
        if (errors[i] > 1) errors[i] = 1;
        if (errors[i] > _thresholdValue) errors[i] = 1;
    }//for
    maxErrorLabel->setNum(maxError);
    if (maxError > finalErrorSpinBox->maximum())
        finalErrorSpinBox->setMaximum(maxError);
}//tick

void QuantiseErrorView::render()
{
    Frame* data = drawQuantised ? dequantised : unquantised;
    if (data == NULL) return;

    //draw points
    float range[] = {_finalColor[0] - _startColor[0],
                     _finalColor[1] - _startColor[1],
                     _finalColor[2] - _startColor[2],
                     _finalColor[3] - _startColor[3]};
    if (_drawLine)
    {
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
    }//if
    else
    {
        glBegin(GL_POINTS);
        for (int i = 0; i < waters.size(); i++)
        {
            if (errors[i] > 1e-6)
            {
                glColor4f(_startColor[0] + errors[i] * range[0],
                          _startColor[1] + errors[i] * range[1],
                          _startColor[2] + errors[i] * range[2],
                          _startColor[3] + errors[i] * range[3]);
                glVertex3f(data->atom_data[3*waters[i].OH2_index],
                           data->atom_data[3*waters[i].OH2_index+1],
                           data->atom_data[3*waters[i].OH2_index+2]);
            }//if
        }//for
        glEnd();
    }//else
}//render


void QuantiseErrorView::setDrawLine(int state)
{
    _drawLine = state != 0;
    settings->setValue("MetaballsView/drawLine", _drawLine);

    lineSizeSlider->setEnabled(_drawLine);
    pointSizeSlider->setDisabled(_drawLine);
}//setDrawLine

void QuantiseErrorView::setPointSize(int value)
{
    if (value > MAX_POINT_SIZE) value = MAX_POINT_SIZE;
    if (value < 1) value = 1;
    _pointSize = value;
    if (current) glPointSize(_pointSize);

    settings->setValue("QuantiseErrorView/pointSize", _pointSize);
}//setPointSize

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
    finalErrorSpinBox->setMinimum(_startErrorValue);

    settings->setValue("QuantiseErrorView/startError", _startErrorValue);
    tick(framenum, unquantised, quantised, dequantised);
}//setStartErrorValue

void QuantiseErrorView::setFinalErrorValue(double value)
{
    _finalErrorValue = value;
    startErrorSpinBox->setMaximum(_finalErrorValue);

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

void QuantiseErrorView::setThresholdValue(int value)
{
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    _thresholdValue = value / 100.0;

    settings->setValue("QuantiseErrorView/thresholdValue", _thresholdValue);
    tick(framenum, unquantised, quantised, dequantised);
}//setThresholdValue

