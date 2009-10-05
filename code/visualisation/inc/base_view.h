#ifndef BASE_VIEW_H
#define BASE_VIEW_H

#include <QObject>
#include <QString>

#include <vector>
#include <pdbio/AtomInformation.h>

class QWidget;

class Frame;
class QuantisedFrame;

class Renderer;

class BaseView : public QObject
{
    Q_OBJECT

    public:
        Renderer* parent;
        QString viewName;
        int viewID;
        int preferenceID;
        QWidget* preferenceParent;
        bool current;
        std::vector<AtomInformation> pdb;

        Frame* frame;
        QuantisedFrame* data;

        BaseView();
        virtual ~BaseView();

        virtual void init(std::vector<AtomInformation> pdb);

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void render();
        virtual void tick(Frame* frame, QuantisedFrame* data);

    signals:
        void selectView(int viewID);

    public slots:
        void select();

    protected:
        virtual void initGL();
        bool pickColor(float* color);

};//BaseView

#endif
