#include "base_view.h"

#include <QColor>
#include <QColorDialog>

BaseView::BaseView()
{
    viewName = "";
    viewID = -1;
    preferenceID = -1;
    preferenceParent = 0;
    current = false;
}//constructor

BaseView::~BaseView()
{
}//destructor


void BaseView::init(std::vector<AtomInformation> pdb)
{
    this->pdb = pdb;
}//init

void BaseView::updatePreferences()
{
}//updatePreferences

QWidget* BaseView::preferenceWidget()
{
    return 0;
}//preferenceWidget


void BaseView::render()
{
}//render

void BaseView::tick(Frame* frame, QuantisedFrame* data)
{
    this->frame = frame;
    this->data = data;
}//tick


void BaseView::select()
{
    if (viewID > -1)
        emit selectView(viewID);
    initGL();
}//select


void BaseView::initGL()
{
}//initGL

bool BaseView::pickColor(float* color)
{
    QColor col = QColorDialog::getColor(
            QColor(
                (int)(color[0] * 255),
                (int)(color[1] * 255),
                (int)(color[2] * 255)),
            preferenceParent);
    if (col.isValid())
    {
        color[0] = (float)col.red() / 255;
        color[1] = (float)col.green() / 255;
        color[2] = (float)col.blue() / 255;
    }//if
    return col.isValid();
}//pickColor

