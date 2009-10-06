#ifndef BALL_STICK_VIEW_H
#define BALL_STICK_VIEW_H

#include "BaseView.h"

#include <GL/glu.h>
#include <splitter/WaterMolecule.h>

class QDoubleSpinBox;
class QSettings;
class QSlider;

class BallStickView : public BaseView
{
    Q_OBJECT

    public:
        BallStickView();
        virtual ~BallStickView();

        virtual void init(std::vector<AtomInformation> pdb);

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void tick(Frame* frame, QuantisedFrame* quantised);
        virtual void render();

    protected:
        virtual void initGL();

    private slots:
        void setHSize(double value);
        void setOSize(double value);
        void pickHColor();
        void pickOColor();
        void setHAlpha(int value);
        void setOAlpha(int value);

    private:
        QSettings* settings;
        float _hColor[4];
        float _oColor[4];
        double _hSize;
        double _oSize;
        GLUquadricObj* quadric;

        std::vector<WaterMolecule> waters;
        std::vector<unsigned int> others;

        QWidget* _preferenceWidget;
        QSlider* hAlphaSlider;
        QSlider* oAlphaSlider;
        QDoubleSpinBox* hSizeSpinBox;
        QDoubleSpinBox* oSizeSpinBox;

        void setupPreferenceWidget();
};//BallStickView

#endif
