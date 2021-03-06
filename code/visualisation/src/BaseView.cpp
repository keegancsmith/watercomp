#include "BaseView.h"

#include <QColor>
#include <QColorDialog>

#include <splitter/FrameSplitter.h>

#include "Renderer.h"

BaseView::BaseView()
{
    parent = 0;
    viewName = "";
    viewID = -1;
    preferenceID = -1;
    _preferenceWidget = 0;

    current = false;

    framenum = -1;
    unquantised = 0;
    quantised = 0;
    dequantised = 0;

    doSplitWaters = false;
    drawQuantised = true;
}//constructor

BaseView::~BaseView()
{
}//destructor


void BaseView::init(std::vector<AtomInformation> pdb)
{
    this->pdb = pdb;
    if (doSplitWaters)
    {
        waters.clear();
        others.clear();
        split_frame(pdb, waters, others);
    }//if
}//init

void BaseView::updatePreferences()
{
}//updatePreferences

QWidget* BaseView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
    {
        _preferenceWidget = new QWidget;
        setupPreferenceWidget(_preferenceWidget);
    }//if
    return _preferenceWidget;
}//preferenceWidget

void BaseView::setupPreferenceWidget(QWidget* preferenceWidget)
{
}//setupPreferenceWidget

void BaseView::select()
{
    if (viewID > -1)
        emit selectView(viewID);
    initGL();
}//select


void BaseView::initGL()
{
}//initGL

void BaseView::tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised)
{
    this->framenum = framenum;
    this->unquantised = unquantised;
    this->quantised = quantised;
    this->dequantised = dequantised;
}//tick

void BaseView::render()
{
}//render


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

