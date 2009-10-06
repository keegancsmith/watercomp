#include "BallStickView.h"

#include <cmath>
#include <cstdio>

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QWidget>
#include <GL/gl.h>

#include <quantiser/QuantisedFrame.h>
#include <splitter/FrameSplitter.h>

#include "Renderer.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 1.0

#define MAX_SPHERE_SIZE 20

BallStickView::BallStickView()
{
    settings = new QSettings;
    viewName = "Ball and stick view";
    _hColor[0] = settings->value("BallStickView/hColorR", 0.0).toDouble();
    _hColor[1] = settings->value("BallStickView/hColorG", 0.0).toDouble();
    _hColor[2] = settings->value("BallStickView/hColorB", 1.0).toDouble();
    _hColor[3] = settings->value("BallStickView/hColorA", 1.0).toDouble();
    _oColor[0] = settings->value("BallStickView/oColorR", 1.0).toDouble();
    _oColor[1] = settings->value("BallStickView/oColorG", 0.0).toDouble();
    _oColor[2] = settings->value("BallStickView/oColorB", 0.0).toDouble();
    _oColor[3] = settings->value("BallStickView/oColorA", 1.0).toDouble();
    quantised = 0;
    _preferenceWidget = NULL;
    _hSize = settings->value("BallStickView/hSize", 10).toDouble();
    _oSize = settings->value("BallStickView/oSize", 10).toDouble();
    quadric = gluNewQuadric();
    // gluQuadricDrawStyle(quadric, GLU_SILHOUETTE);
}//constructor

BallStickView::~BallStickView()
{
    settings->sync();
    delete settings;
    gluDeleteQuadric(quadric);
}//destructor


void BallStickView::init(std::vector<AtomInformation> pdb)
{
    this->pdb = pdb;
    FrameSplitter::split_frame(pdb, waters, others);
}//init


void BallStickView::updatePreferences()
{
    hSizeSpinBox->setValue(_hSize);
    oSizeSpinBox->setValue(_oSize);
    hAlphaSlider->setValue((int)(_hColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    oAlphaSlider->setValue((int)(_oColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
}//updatePreferences

QWidget* BallStickView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
        setupPreferenceWidget();
    return _preferenceWidget;
}//preferenceWidget


void BallStickView::tick(Frame* frame, QuantisedFrame* quantised)
{
    this->frame = frame;
    this->quantised = quantised;
}//tick

void BallStickView::render()
{
    if (quantised == NULL)
        return;
    if (parent)
        glTranslatef(-parent->volume_middle[0], -parent->volume_middle[1], -parent->volume_middle[2]);

    float l_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);

    float l2_pos[] = {-1.0f, -1.0f, -1.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, l2_pos);

    int n = 0;
    int oslice = (_oSize * 2 * M_PI);
    int hslice = (_hSize * 2 * M_PI);
    if (oslice < 1) oslice = 1;
    if (hslice < 1) hslice = 1;
    for (int i = 0; i < waters.size(); i++)
    {
        if (n++ > 100) break;
        glColor4fv(_oColor);
        glPushMatrix();
        glTranslatef(quantised->quantised_frame[3*waters[i].OH2_index],
                     quantised->quantised_frame[3*waters[i].OH2_index+1],
                     quantised->quantised_frame[3*waters[i].OH2_index+2]);
        gluSphere(quadric, _oSize, oslice, oslice);
        glPopMatrix();

        glColor4fv(_hColor);
        glPushMatrix();
        glTranslatef(quantised->quantised_frame[3*waters[i].H1_index],
                     quantised->quantised_frame[3*waters[i].H1_index+1],
                     quantised->quantised_frame[3*waters[i].H1_index+2]);
        gluSphere(quadric, _hSize, hslice, hslice);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(quantised->quantised_frame[3*waters[i].H2_index],
                     quantised->quantised_frame[3*waters[i].H2_index+1],
                     quantised->quantised_frame[3*waters[i].H2_index+2]);
        gluSphere(quadric, _hSize, hslice, hslice);
        glPopMatrix();
    }//for
}//render


void BallStickView::initGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}//initGL


void BallStickView::setHAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _hColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;

    settings->setValue("BallStickView/hColorA", _hColor[3]);
}//setHAlpha

void BallStickView::setOAlpha(int value)
{
    if (value > MAX_ALPHA_SLIDER) value = MAX_ALPHA_SLIDER;
    if (value < 0) value = 0;
    _oColor[3] = (float)value * MAX_ALPHA_VAL / MAX_ALPHA_SLIDER;

    settings->setValue("BallStickView/oColorA", _oColor[3]);
}//setOAlpha

void BallStickView::setHSize(double value)
{
    if (value > MAX_SPHERE_SIZE) value = MAX_SPHERE_SIZE;
    if (value < 1) value = 1;
    _hSize = value;

    settings->setValue("BallStickView/hSize", _hSize);
}//setHSize

void BallStickView::setOSize(double value)
{
    if (value > MAX_SPHERE_SIZE) value = MAX_SPHERE_SIZE;
    if (value < 1) value = 1;
    _oSize = value;

    settings->setValue("BallStickView/OSize", _oSize);
}//setOSize

void BallStickView::pickHColor()
{
    pickColor(_hColor);

    settings->setValue("BallStickView/hColorR", _hColor[0]);
    settings->setValue("BallStickView/hColorG", _hColor[1]);
    settings->setValue("BallStickView/hColorB", _hColor[2]);
}//pickHColor

void BallStickView::pickOColor()
{
    pickColor(_oColor);

    settings->setValue("BallStickView/oColorR", _oColor[0]);
    settings->setValue("BallStickView/oColorG", _oColor[1]);
    settings->setValue("BallStickView/oColorB", _oColor[2]);
}//pickOColor


void BallStickView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;

    QGridLayout* layout = new QGridLayout(_preferenceWidget);

    QPushButton* hColorButton = new QPushButton(tr("Select hydrogen colour"), _preferenceWidget);
    connect(hColorButton, SIGNAL(clicked()), this, SLOT(pickHColor()));
    layout->addWidget(hColorButton, 0, 0, 1, 2);

    QLabel* hAlphaLabel = new QLabel(tr("Hydrogen alpha"), _preferenceWidget);
    layout->addWidget(hAlphaLabel, 1, 0);

    hAlphaSlider = new QSlider(_preferenceWidget);
    hAlphaSlider->setOrientation(Qt::Horizontal);
    hAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(hAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setHAlpha(int)));
    layout->addWidget(hAlphaSlider, 1, 1);

    QLabel* hSizeLabel = new QLabel(tr("Hydrogen size"), _preferenceWidget);
    layout->addWidget(hSizeLabel, 2, 0);

    hSizeSpinBox = new QDoubleSpinBox(_preferenceWidget);
    hSizeSpinBox->setRange(0, MAX_SPHERE_SIZE);
    connect(hSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHSize(double)));
    layout->addWidget(hSizeSpinBox, 2, 1);

    QPushButton* oColorButton = new QPushButton(tr("Select oxygen colour"), _preferenceWidget);
    connect(oColorButton, SIGNAL(clicked()), this, SLOT(pickOColor()));
    layout->addWidget(oColorButton, 3, 0, 1, 2);

    QLabel* oAlphaLabel = new QLabel(tr("Oxygen alpha"), _preferenceWidget);
    layout->addWidget(oAlphaLabel, 4, 0);

    oAlphaSlider = new QSlider(_preferenceWidget);
    oAlphaSlider->setOrientation(Qt::Horizontal);
    oAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(oAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setOAlpha(int)));
    layout->addWidget(oAlphaSlider, 4, 1);

    QLabel* oSizeLabel = new QLabel(tr("Oxygen size"), _preferenceWidget);
    layout->addWidget(oSizeLabel, 5, 0);

    oSizeSpinBox = new QDoubleSpinBox(_preferenceWidget);
    oSizeSpinBox->setRange(0, MAX_SPHERE_SIZE);
    connect(oSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setOSize(double)));
    layout->addWidget(oSizeSpinBox, 5, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

