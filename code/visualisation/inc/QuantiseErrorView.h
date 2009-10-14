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

        virtual void tick(int framenum, Frame* frame, QuantisedFrame* quantised, Frame* dequantised);
        virtual void render();

    protected:
        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

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
        float* errors;
        int _lineSize;
        float _startColor[4];
        float _finalColor[4];
        double _startErrorValue;
        double _finalErrorValue;

        QSlider* lineSizeSlider;
        QSlider* startAlphaSlider;
        QSlider* finalAlphaSlider;
        QDoubleSpinBox* startErrorSpinBox;
        QDoubleSpinBox* finalErrorSpinBox;

};//QuantiseErrorView

#endif
