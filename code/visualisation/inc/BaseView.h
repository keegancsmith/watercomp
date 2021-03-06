#ifndef BASE_VIEW_H
#define BASE_VIEW_H

#include <QObject>
#include <QString>

#include <vector>
#include <pdbio/AtomInformation.h>
#include <splitter/WaterMolecule.h>

class QWidget;

class Frame;
class QuantisedFrame;

class Renderer;

class BaseView : public QObject
{
    Q_OBJECT

    public:
        Renderer* parent;
        QWidget* preferenceParent;
        QString viewName;
        int viewID;
        int preferenceID;
        std::vector<AtomInformation> pdb;

        bool current;
        int framenum;
        Frame* unquantised;
        QuantisedFrame* quantised;
        Frame* dequantised;
        bool drawQuantised;

        BaseView();
        virtual ~BaseView();

        virtual void init(std::vector<AtomInformation> pdb);

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised);
        virtual void render();

    signals:
        void selectView(int viewID);

    public slots:
        void select();

    protected:
        QWidget* _preferenceWidget;
        bool doSplitWaters;
        std::vector<WaterMolecule> waters;
        std::vector<unsigned int> others;

        bool pickColor(float* color);

        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

};//BaseView

#endif
