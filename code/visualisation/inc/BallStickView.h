#ifndef BALL_STICK_VIEW_H
#define BALL_STICK_VIEW_H

#include "BaseView.h"

#include <GL/glu.h>
#include <splitter/WaterMolecule.h>

class QCheckBox;
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

        virtual void tick(int framenum, Frame* frame, QuantisedFrame* quantised);
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
        void setLighting(int state);

    private:
        QSettings* settings;
        float _hColor[4];
        float _oColor[4];
        double _hSize;
        double _oSize;
        int hSliceCount;
        int oSliceCount;
        bool lighting;
        GLUquadricObj* quadric;

        std::vector<WaterMolecule> waters;
        std::vector<unsigned int> others;

        QWidget* _preferenceWidget;
        QSlider* hAlphaSlider;
        QSlider* oAlphaSlider;
        QDoubleSpinBox* hSizeSpinBox;
        QDoubleSpinBox* oSizeSpinBox;
        QCheckBox* lightCheckBox;

        void setupPreferenceWidget();
};//BallStickView

#endif
