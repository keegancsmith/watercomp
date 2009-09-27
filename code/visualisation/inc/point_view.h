#ifndef POINT_VIEW_H
#define POINT_VIEW_H

#include "base_view.h"

class QSettings;
class QSlider;

class PointView : public BaseView
{
    Q_OBJECT

    public:
        PointView();
        virtual ~PointView();

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void tick(Frame_Data* data);
        virtual void render();

    protected:
        virtual void initGL();

    private slots:
        void setPointAlpha(int value);
        void setPointSize(int value);
        void pickPointColor();

    private:
        QSettings* settings;
        Frame_Data* data;
        float _pointColor[4];
        int _pointSize;

        QWidget* _preferenceWidget;
        QSlider* pointSizeSlider;
        QSlider* pointAlphaSlider;

        void setupPreferenceWidget();
};//PointView

#endif
