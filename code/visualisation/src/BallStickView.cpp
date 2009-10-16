#include "BallStickView.h"

#include <cmath>
#include <cstdio>

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

#include "Renderer.h"
#include "Util.h"

#define MAX_ALPHA_SLIDER 100
#define MAX_ALPHA_VAL 1.0

#define MIN_SPHERE_SIZE 0.1
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
    _hSize = settings->value("BallStickView/hSize", 1).toDouble();
    _oSize = settings->value("BallStickView/oSize", 2).toDouble();
    setHSize(_hSize);
    setOSize(_oSize);
    lighting = settings->value("BallStickView/lighting", false).toBool();
    quadric = gluNewQuadric();
    number = settings->value("BallStickView/number", 50).toInt();
    // gluQuadricDrawStyle(quadric, GLU_SILHOUETTE);
    doSplitWaters = true;
}//constructor

BallStickView::~BallStickView()
{
    settings->sync();
    delete settings;
    gluDeleteQuadric(quadric);
}//destructor


void BallStickView::init(std::vector<AtomInformation> pdb)
{
    BaseView::init(pdb);
    numberBox->setRange(0, waters.size());
}//init


void BallStickView::updatePreferences()
{
    hSizeSpinBox->setValue(_hSize);
    oSizeSpinBox->setValue(_oSize);
    hAlphaSlider->setValue((int)(_hColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    oAlphaSlider->setValue((int)(_oColor[3] * MAX_ALPHA_SLIDER / MAX_ALPHA_VAL));
    lightCheckBox->setCheckState(lighting ? Qt::Checked : Qt::Unchecked);
    numberBox->setValue(number);
}//updatePreferences

void BallStickView::setupPreferenceWidget(QWidget* preferenceWidget)
{
    QGridLayout* layout = new QGridLayout(preferenceWidget);

    QPushButton* hColorButton = new QPushButton(tr("Select hydrogen colour"), preferenceWidget);
    connect(hColorButton, SIGNAL(clicked()), this, SLOT(pickHColor()));
    layout->addWidget(hColorButton, 0, 0, 1, 2);

    QLabel* hAlphaLabel = new QLabel(tr("Hydrogen alpha"), preferenceWidget);
    layout->addWidget(hAlphaLabel, 1, 0);

    hAlphaSlider = new QSlider(preferenceWidget);
    hAlphaSlider->setOrientation(Qt::Horizontal);
    hAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(hAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setHAlpha(int)));
    layout->addWidget(hAlphaSlider, 1, 1);

    QLabel* hSizeLabel = new QLabel(tr("Hydrogen size"), preferenceWidget);
    layout->addWidget(hSizeLabel, 2, 0);

    hSizeSpinBox = new QDoubleSpinBox(preferenceWidget);
    hSizeSpinBox->setRange(0, MAX_SPHERE_SIZE);
    connect(hSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setHSize(double)));
    layout->addWidget(hSizeSpinBox, 2, 1);

    QPushButton* oColorButton = new QPushButton(tr("Select oxygen colour"), preferenceWidget);
    connect(oColorButton, SIGNAL(clicked()), this, SLOT(pickOColor()));
    layout->addWidget(oColorButton, 3, 0, 1, 2);

    QLabel* oAlphaLabel = new QLabel(tr("Oxygen alpha"), preferenceWidget);
    layout->addWidget(oAlphaLabel, 4, 0);

    oAlphaSlider = new QSlider(preferenceWidget);
    oAlphaSlider->setOrientation(Qt::Horizontal);
    oAlphaSlider->setRange(0, MAX_ALPHA_SLIDER);
    connect(oAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setOAlpha(int)));
    layout->addWidget(oAlphaSlider, 4, 1);

    QLabel* oSizeLabel = new QLabel(tr("Oxygen size"), preferenceWidget);
    layout->addWidget(oSizeLabel, 5, 0);

    oSizeSpinBox = new QDoubleSpinBox(preferenceWidget);
    oSizeSpinBox->setRange(0, MAX_SPHERE_SIZE);
    connect(oSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setOSize(double)));
    layout->addWidget(oSizeSpinBox, 5, 1);

    QLabel* lightLabel = new QLabel(tr("Enable lighting"), preferenceWidget);
    layout->addWidget(lightLabel, 6, 0);

    lightCheckBox = new QCheckBox(preferenceWidget);
    connect(lightCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLighting(int)));
    layout->addWidget(lightCheckBox, 6, 1);

    QLabel* numberLabel = new QLabel(tr("Molecule count"), preferenceWidget);
    layout->addWidget(numberLabel, 7, 0);

    numberBox = new QSpinBox(preferenceWidget);
    numberBox->setRange(0, 0);
    connect(numberBox, SIGNAL(valueChanged(int)), this, SLOT(setNumber(int)));
    layout->addWidget(numberBox, 7, 1);

    preferenceWidget->setLayout(layout);
}//setupPreferenceWidget


void BallStickView::initGL()
{
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

void BallStickView::render()
{
    if (dequantised == NULL) return;

    float l0_pos[] = {-1.0f, 1.0f, 2.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

    float l1_pos[] = {0.0f, 0.0f, -1.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);

    int n = 0;
    int oslice = oSliceCount * 2;
    int hslice = hSliceCount * 2;
    float pos[3];
    float dis_ratio = 1;

    /*
    for (int a = 0; a < 3; a++)
        pos[a] = quantised->quantised_frame[3*waters[i].OH2_index + a] - parent->volume_middle[a];
    pos[2] += parent->zoom();
    dis_ratio = 100000 / len2(pos);
    oslice = oSliceCount * _oSize * _oSize * dis_ratio;
    hslice = hSliceCount * _hSize * _hSize * dis_ratio;
    // oslice = oSliceCount;
    // hslice = hSliceCount;
    // printf("slice: %i %i\n", oslice, hslice);
    // */
    if (oslice < 4) oslice = 4;
    if (hslice < 4) hslice = 4;

    for (int i = 0; i < waters.size(); i++)
    {
        if (n++ > this->number) break;
        glColor4fv(_oColor);
        glPushMatrix();
        glTranslatef(dequantised->atom_data[3*waters[i].OH2_index],
                     dequantised->atom_data[3*waters[i].OH2_index+1],
                     dequantised->atom_data[3*waters[i].OH2_index+2]);
        gluSphere(quadric, _oSize, oslice, oslice);
        glPopMatrix();

        glColor4fv(_hColor);
        glPushMatrix();
        glTranslatef(dequantised->atom_data[3*waters[i].H1_index],
                     dequantised->atom_data[3*waters[i].H1_index+1],
                     dequantised->atom_data[3*waters[i].H1_index+2]);
        gluSphere(quadric, _hSize, hslice, hslice);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(dequantised->atom_data[3*waters[i].H2_index],
                     dequantised->atom_data[3*waters[i].H2_index+1],
                     dequantised->atom_data[3*waters[i].H2_index+2]);
        gluSphere(quadric, _hSize, hslice, hslice);
        glPopMatrix();
    }//for
}//render


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
    if (value < MIN_SPHERE_SIZE) value = MIN_SPHERE_SIZE;
    _hSize = value;
    hSliceCount = (_hSize * 2 * M_PI);

    settings->setValue("BallStickView/hSize", _hSize);
}//setHSize

void BallStickView::setOSize(double value)
{
    if (value > MAX_SPHERE_SIZE) value = MAX_SPHERE_SIZE;
    if (value < MIN_SPHERE_SIZE) value = MIN_SPHERE_SIZE;
    _oSize = value;
    oSliceCount = (_oSize * 2 * M_PI);

    settings->setValue("BallStickView/oSize", _oSize);
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

void BallStickView::setLighting(int state)
{
    lighting = state != 0;
    settings->setValue("BallStickView/lighting", lighting);

    if (current)
    {
        if (lighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
    }//if
}//setLighting

void BallStickView::setNumber(int value)
{
    number = value;
    settings->setValue("BallStickView/number", number);
}//setNumber

