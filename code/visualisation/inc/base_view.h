#ifndef BASE_VIEW_H
#define BASE_VIEW_H

#include <QObject>
#include <QString>

class Frame_Data;
class QWidget;

class BaseView : public QObject
{
    Q_OBJECT

    public:
        QString viewName;
        int viewID;
        int preferenceID;
        QWidget* preferenceParent;

        BaseView();
        virtual ~BaseView();

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void render();
        virtual void tick(Frame_Data* data);

    signals:
        void selectView(int viewID);

    public slots:
        void select();

    protected:
        virtual void initGL();
        bool pickColor(float* color);

};//BaseView

#endif
