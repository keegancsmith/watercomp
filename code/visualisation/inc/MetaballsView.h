#ifndef METABALLS_VIEW_H
#define METABALLS_VIEW_H

#include "BaseView.h"

#include <gts.h>
#include <map>
#include <QVector>

class QSettings;
class QSlider;

struct Point3f
{
    float x;
    float y;
    float z;

    bool operator<(const Point3f& p) const;
};//Point3f

struct Triangle
{
    float p[3][3]; // 3 positions
    float n[3][3]; // 3 normals
};//Triangle


class MetaballsView : public BaseView
{
    Q_OBJECT

    public:
        // std::map<GtsPoint*, int> vertex_map;
        std::map<Point3f, int> vertex_map;
        int vertex_num;
        int triangle_num;
        float* vertices;
        float* normals;
        float* avg_normals;
        int* indices;

        MetaballsView();
        virtual ~MetaballsView();

        int stepSize();

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void render();
        virtual void tick(Frame* frame, QuantisedFrame* quantised);

    protected:
        virtual void initGL();

    private slots:
        void setMetaballsAlpha(int value);
        void setStepSize(int value);
        void pickMetaballsColor();

        void setCullFace(int state);
        void setLighting(int state);
        void updateFaces();

    private:
        QSettings* settings;
        bool cullFace;
        bool lighting;
        int maxStepSize;
        int _stepSize;
        float _metaballsColor[4];
        QVector<Triangle> _surface;

        unsigned char*** volumedata;

        QWidget* _preferenceWidget;
        QSlider* metaballsAlphaSlider;
        QSlider* stepSizeSlider;

        void init();

        void setupPreferenceWidget();

        float sampleVolume(float x, float y, float z);
        void getNormal(float* normal, float x, float y, float z);
        float getOffset(float f1, float f2, float value);
        void doMarchingTetrahedron(float tetrahedronPosition[4][3], float tetrahedronValue[3]);
        void callMarchingTetrahedrons(float x, float y, float z, float scale);
        void callMarchingCubes(float x, float y, float z, float scale);

        GtsSurface* g_surface;
        GtsCartesianGrid g_grid;

};//MetaballsView

#endif
