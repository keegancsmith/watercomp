#ifndef QUANTISE_ERROR_VIEW_H
#define QUANTISE_ERROR_VIEW_H

#include "BaseView.h"

#include <pdbio/Frame.h>

class QDoubleSpinBox;
class QSettings;
class QSlider;

class QuantiseErrorView : public BaseView
{
    Q_OBJECT

    public:
        QuantiseErrorView();
        virtual ~QuantiseErrorView();

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void tick(int framenum, Frame* frame, QuantisedFrame* quantised);
        virtual void render();

    protected:
        virtual void initGL();

    private slots:
        void setLineSize(int value);
        void setStartErrorValue(double value);
        void setFinalErrorValue(double value);
        void pickStartColor();
        void pickFinalColor();
        void setStartAlpha(int value);
        void setFinalAlpha(int value);

    private:
        QSettings* settings;
        Frame* dequantised;
        int _lineSize;
        float _startColor[4];
        float _finalColor[4];
        double _startErrorValue;
        double _finalErrorValue;

        QWidget* _preferenceWidget;
        QSlider* lineSizeSlider;
        QSlider* startAlphaSlider;
        QSlider* finalAlphaSlider;
        QDoubleSpinBox* startErrorSpinBox;
        QDoubleSpinBox* finalErrorSpinBox;

        void setupPreferenceWidget();
};//QuantiseErrorView

#endif
