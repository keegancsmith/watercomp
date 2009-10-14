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
        void pickHColor();
        void pickOColor();
        void setHAlpha(int value);
        void setOAlpha(int value);
        void setLighting(int state);
        void setNumber(int value);

    private:
        QSettings* settings;
        float _hColor[4];
        float _oColor[4];
        double _hSize;
        double _oSize;
        int hSliceCount;
        int oSliceCount;
        bool lighting;
        int number;
        GLUquadricObj* quadric;

        std::vector<WaterMolecule> waters;
        std::vector<unsigned int> others;

        QWidget* _preferenceWidget;
        QSlider* hAlphaSlider;
        QSlider* oAlphaSlider;
        QDoubleSpinBox* hSizeSpinBox;
        QDoubleSpinBox* oSizeSpinBox;
        QCheckBox* lightCheckBox;
        QSpinBox* numberBox;

};//BallStickView

#endif
