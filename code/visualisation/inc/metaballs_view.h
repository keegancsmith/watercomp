#ifndef METABALLS_VIEW_H
#define METABALLS_VIEW_H

#include "base_view.h"

#include <QVector>

class QGridLayout;
class QLabel;
class QPushButton;
class QSlider;


struct Point3f
{
    float p[3];
};//Point3f

struct Triangle
{
    Point3f p[3]; // 3 positions
    Point3f n[3]; // 3 normals
};//Triangle

struct GridCell
{
    Point3f p[8]; // 8 positions
    Point3f n[8]; // 8 normals
    float val[8]; // 8 values
};//GridCell


class MetaballsView : public BaseView
{
    Q_OBJECT

    public:
        MetaballsView();
        virtual ~MetaballsView();

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void render();
        virtual void tick(Frame_Data* data);

    private slots:
        void setMetaballsAlpha(int value);
        void pickMetaballsColor();

    private:
        Frame_Data* data;
        float _metaballsColor[4];
        QVector<Triangle> _surface;

        QWidget* _preferenceWidget;
        QGridLayout* layout;
        QPushButton* metaballsColorButton;
        QLabel* metaballsAlphaLabel;
        QSlider* metaballsAlphaSlider;

        void init();

        void setupPreferenceWidget();

        void fillGridCell(GridCell& grid, unsigned char*** data, int i, int x, int y, int z);
        void addTriangle(QVector<Triangle>& surface, GridCell& g, int iso, int v0, int v1, int v2, int v3, int v4, int v5);
        void marchTetrahedron(QVector<Triangle>& surface, GridCell& grid, int iso, int v0, int v1, int v2, int v3);
        Point3f vertexInterpolate(float iso, GridCell& g, int v1, int v2);
};//MetaballsView

#endif
