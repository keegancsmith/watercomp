#ifndef QUANTISE_ERROR_VIEW_H
#define QUANTISE_ERROR_VIEW_H

#include "base_view.h"

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

        virtual void tick(Frame* frame, QuantisedFrame* data);
        virtual void render();

    protected:
        virtual void initGL();

    private slots:
        void setLineAlpha(int value);
        void setLineSize(int value);
        void setErrorValue(double value);
        void pickLineColor();

    private:
        QSettings* settings;
        Frame* dequantised;
        float _lineColor[4];
        int _lineSize;
        double _errorValue;

        QWidget* _preferenceWidget;
        QSlider* lineSizeSlider;
        QSlider* lineAlphaSlider;
        QDoubleSpinBox* errorSpinBox;

        void setupPreferenceWidget();
};//QuantiseErrorView

#endif
