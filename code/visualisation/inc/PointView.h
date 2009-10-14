#ifndef POINT_VIEW_H
#define POINT_VIEW_H

#include "BaseView.h"

class QSettings;
class QSlider;

class PointView : public BaseView
{
    Q_OBJECT

    public:
        PointView();
        virtual ~PointView();

        virtual void updatePreferences();

        virtual void render();

    protected:
        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

    private slots:
        void setPointAlpha(int value);
        void setPointSize(int value);
        void pickPointColor();

    private:
        QSettings* settings;
        float _pointColor[4];
        int _pointSize;

        QSlider* pointSizeSlider;
        QSlider* pointAlphaSlider;

};//PointView

#endif
