#ifndef QUANTISE_ERROR_VIEW_H
#define QUANTISE_ERROR_VIEW_H

#include "BaseView.h"

#include <pdbio/Frame.h>

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QSettings;
class QSlider;

class QuantiseErrorView : public BaseView
{
    Q_OBJECT

    public:
        QuantiseErrorView();
        virtual ~QuantiseErrorView();

        virtual void updatePreferences();

        virtual void tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised);
        virtual void render();

    protected:
        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

    private slots:
        void setDrawLine(int state);
        void setPointSize(int value);
        void setLineSize(int value);
        void setStartErrorValue(double value);
        void setFinalErrorValue(double value);
        void pickStartColor();
        void pickFinalColor();
        void setStartAlpha(int value);
        void setFinalAlpha(int value);
        void setThresholdValue(int value);

    private:
        QSettings* settings;
        float* errors;
        int _lineSize;
        int _pointSize;
        float _startColor[4];
        float _finalColor[4];
        double _startErrorValue;
        double _finalErrorValue;
        double _thresholdValue;
        bool _drawLine;

        QSlider* pointSizeSlider;
        QSlider* lineSizeSlider;
        QSlider* startAlphaSlider;
        QSlider* finalAlphaSlider;
        QDoubleSpinBox* startErrorSpinBox;
        QDoubleSpinBox* finalErrorSpinBox;
        QSlider* thresholdSlider;
        QLabel* maxErrorLabel;
        QCheckBox* drawLineCheckBox;

};//QuantiseErrorView

#endif
