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
        QString menuItemName;
        int viewID;

        BaseView();
        virtual ~BaseView();

        virtual QWidget* preferenceWidget();
        virtual void render();
        virtual void tick(Frame_Data* data);

    signals:
        void selectView(int viewID);

    public slots:
        void select();
};//BaseView

#endif
