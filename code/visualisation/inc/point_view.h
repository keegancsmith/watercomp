#ifndef POINT_VIEW_H
#define POINT_VIEW_H

#include "base_view.h"

class QSlider;
class QVBoxLayout;

class PointView : public BaseView
{
    public:
        PointView();
        virtual ~PointView();

        virtual QWidget* preferenceWidget();

        virtual void tick(Frame_Data* data);
        virtual void render();

    private:
        Frame_Data* data;
        QWidget* _preferenceWidget;
        float _pointColor[4];

        void setupPreferenceWidget();
};//PointView

#endif
