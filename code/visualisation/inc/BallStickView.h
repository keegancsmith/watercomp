#ifndef BALL_STICK_VIEW_H
#define BALL_STICK_VIEW_H

#include "BaseView.h"

#include <GL/glu.h>
#include <splitter/WaterMolecule.h>

class QCheckBox;
class QDoubleSpinBox;
class QSettings;
class QSlider;
class QSpinBox;

class BallStickView : public BaseView
{
    Q_OBJECT

    public:
        BallStickView();
        virtual ~BallStickView();

        virtual void init(std::vector<AtomInformation> pdb);

        virtual void updatePreferences();

        virtual void render();

    protected:
        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

    private slots:
        void setHSize(double value);
        void setOSize(double value);
        void setStickSize(double value);
        void pickHColor();
        void pickOColor();
        void pickStickColor();
        void setHAlpha(int value);
        void setOAlpha(int value);
        void setStickAlpha(int value);
        void setLighting(int state);
        void setDrawStick(int state);
        void setNumber(int value);

    private:
        QSettings* settings;
        float _hColor[4];
        float _oColor[4];
        float _stickColor[4];
        double _hSize;
        double _oSize;
        double _stickSize;
        int hSliceCount;
        int oSliceCount;
        int stickSliceCount;
        bool lighting;
        int number;
        GLUquadricObj* quadric;
        bool drawStick;

        QWidget* _preferenceWidget;
        QSlider* hAlphaSlider;
        QSlider* oAlphaSlider;
        QSlider* stickAlphaSlider;
        QDoubleSpinBox* hSizeSpinBox;
        QDoubleSpinBox* oSizeSpinBox;
        QDoubleSpinBox* stickSizeSpinBox;
        QCheckBox* lightCheckBox;
        QCheckBox* drawStickCheckBox;
        QSpinBox* numberBox;

};//BallStickView

#endif
