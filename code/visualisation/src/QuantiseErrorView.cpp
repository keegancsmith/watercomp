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
    _lineColor[0] = settings->value("QuantiseErrorView/colorR", 1.0).toDouble();
    _lineColor[1] = settings->value("QuantiseErrorView/colorG", 0.0).toDouble();
    _lineColor[2] = settings->value("QuantiseErrorView/colorB", 0.0).toDouble();
    _lineColor[3] = settings->value("QuantiseErrorView/colorA", 0.8).toDouble();
    _preferenceWidget = NULL;
    quantised = 0;
    dequantised = 0;
    _lineSize = settings->value("QuantiseErrorView/lineSize", 2).toInt();
    _errorValue = settings->value("QuantiseErrorView/error", 0.06).toDouble();
}//constructor

QuantiseErrorView::~QuantiseErrorView()
{
    settings->sync();
    delete settings;
}//destructor


void QuantiseErrorView::updatePreferences()
{
    lineSizeSlider->setValue(_lineSize);
    lineAlphaSlider->setValue((int)(_lineColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    errorSpinBox->setValue(_errorValue);
}//updatePreferences

QWidget* QuantiseErrorView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
        setupPreferenceWidget();
    return _preferenceWidget;
}//preferenceWidget


void QuantiseErrorView::tick(int framenum, Frame* frame, QuantisedFrame* quantised)
{
    this->frame = frame;
    this->quantised = quantised;
    if (quantised == NULL) return;

    // printf("frame size: %i\n", frame->atom_data.size());
    // printf("quant size: %i\n", data->toFrame().atom_data.size());
    if (dequantised) delete dequantised;
    dequantised = new Frame(quantised->toFrame());
    // printf("deqnt size: %i\n", dequantised->atom_data.size());
}//tick

void QuantiseErrorView::render()
{
    if (quantised == NULL)
        return;

    //draw points
    float dif[3];
    float d;
    glBegin(GL_LINES);
    for (int i = 0; i < quantised->natoms(); i++)
    {
        if (pdb[i].atom_name == "OH2")
        {
            dif[0] = dequantised->atom_data[i*3]   - frame->atom_data[i*3];
            dif[1] = dequantised->atom_data[i*3+1] - frame->atom_data[i*3+1];
            dif[2] = dequantised->atom_data[i*3+2] - frame->atom_data[i*3+2];
            d = len2(dif);
            d *= d;
            d = d / _errorValue;
            if (d > 1) d = 1;
            if (d > 1e-6)
            {
                glColor4f(_lineColor[0], _lineColor[1], _lineColor[2], _lineColor[3]*d);
                glVertex3f(frame->atom_data[i*3],
                           frame->atom_data[i*3+1],
                           frame->atom_data[i*3+2]);
                glVertex3f(dequantised->atom_data[i*3],
                           dequantised->atom_data[i*3+1],
                           dequantised->atom_data[i*3+2]);
            }//if
        }//if
    }//for
    glEnd();
}//render


void QuantiseErrorView::initGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(_lineSize);

    glDisable(GL_LIGHTING);
}//initGL


void QuantiseErrorView::setLineAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _lineColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;

    settings->setValue("QuantiseErrorView/colorA", _lineColor[3]);
}//setLineAlpha

void QuantiseErrorView::setLineSize(int value)
{
    if (value > MAX_LINE_WIDTH) value = MAX_LINE_WIDTH;
    if (value < 1) value = 1;
    _lineSize = value;
    if (current) glLineWidth(_lineSize);

    settings->setValue("QuantiseErrorView/lineSize", _lineSize);
}//setLineSize

void QuantiseErrorView::setErrorValue(double value)
{
    _errorValue = value;

    settings->setValue("QuantiseErrorView/error", _errorValue);
}//setErrorValue

void QuantiseErrorView::pickLineColor()
{
    pickColor(_lineColor);

    settings->setValue("QuantiseErrorView/colorR", _lineColor[0]);
    settings->setValue("QuantiseErrorView/colorG", _lineColor[1]);
    settings->setValue("QuantiseErrorView/colorB", _lineColor[2]);
}//pickLineColor


void QuantiseErrorView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;

    QGridLayout* layout = new QGridLayout(_preferenceWidget);

    QPushButton* lineColorButton = new QPushButton(tr("Select line colour"), _preferenceWidget);
    connect(lineColorButton, SIGNAL(clicked()), this, SLOT(pickLineColor()));
    layout->addWidget(lineColorButton, 0, 0, 1, 2);

    QLabel* lineSizeLabel = new QLabel(tr("Line width"), _preferenceWidget);
    layout->addWidget(lineSizeLabel, 1, 0);

    lineSizeSlider = new QSlider(_preferenceWidget);
    lineSizeSlider->setOrientation(Qt::Horizontal);
    lineSizeSlider->setRange(0, MAX_LINE_WIDTH);
    connect(lineSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineSize(int)));
    layout->addWidget(lineSizeSlider, 1, 1);

    QLabel* lineAlphaLabel = new QLabel(tr("Line alpha"), _preferenceWidget);
    layout->addWidget(lineAlphaLabel, 2, 0);

    lineAlphaSlider = new QSlider(_preferenceWidget);
    lineAlphaSlider->setOrientation(Qt::Horizontal);
    lineAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(lineAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setLineAlpha(int)));
    layout->addWidget(lineAlphaSlider, 2, 1);

    QLabel* errorLabel = new QLabel(tr("Error level"), _preferenceWidget);
    layout->addWidget(errorLabel, 3, 0);

    errorSpinBox = new QDoubleSpinBox(_preferenceWidget);
    errorSpinBox->setRange(0, 1.0);
    errorSpinBox->setDecimals(3);
    errorSpinBox->setSingleStep(0.01);
    connect(errorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setErrorValue(double)));
    layout->addWidget(errorSpinBox, 3, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

