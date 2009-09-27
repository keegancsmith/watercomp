#ifndef POINT_VIEW_H
#define POINT_VIEW_H

#include "base_view.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QSlider;
class QVBoxLayout;

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
        Frame_Data* data;
        float _pointColor[4];
        int _pointSize;

        QWidget* _preferenceWidget;
        QGridLayout* layout;
        QPushButton* pointColorButton;
        QLabel* pointSizeLabel;
        QSlider* pointSizeSlider;
        QLabel* pointAlphaLabel;
        QSlider* pointAlphaSlider;

        void setupPreferenceWidget();
};//PointView

#endif
