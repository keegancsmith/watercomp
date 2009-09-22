#include "point_view.h"

#include <QWidget>
#include <QSlider>
#include <GL/gl.h>

#include "frame_data.h"

PointView::PointView()
{
    menuItemName = "Point data view";
    _pointColor[0] = 0.0f;
    _pointColor[1] = 0.0f;
    _pointColor[2] = 1.0f;
    _pointColor[3] = 0.02f;
    data = 0;
    setupPreferenceWidget();
}//constructor

PointView::~PointView()
{
    if (data) delete data;
    delete _preferenceWidget;
}//destructor


QWidget* PointView::preferenceWidget()
{
    return _preferenceWidget;
}//preferenceWidget


void PointView::tick(Frame_Data* data)
{
    this->data = data;
}//tick

void PointView::render()
{
    //draw points
    glColor4fv(_pointColor);
    glBegin(GL_POINTS);
    for (int i = 0; i < data->natoms(); i++)
    {
        glVertex3dv(data->at(i).pos);
    }//for
    glEnd();
}//render


void PointView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;
}//setupPreferenceWidget
