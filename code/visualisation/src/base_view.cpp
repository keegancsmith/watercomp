#include "base_view.h"

#include "frame_data.h"

BaseView::BaseView()
{
    menuItemName = "";
    viewID = -1;
}//constructor

BaseView::~BaseView()
{
}//destructor


QWidget* BaseView::preferenceWidget()
{
    return 0;
}//preferenceWidget

void BaseView::render()
{
}//render

void BaseView::tick(Frame_Data* data)
{
}//tick


void BaseView::select()
{
    if (viewID > -1)
        emit selectView(viewID);
}//select
