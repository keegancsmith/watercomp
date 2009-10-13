#ifndef METABALLS_VIEW_H
#define METABALLS_VIEW_H

#include "BaseView.h"

#include <gts.h>
#include <map>
#include <QVector>

class QCheckBox;
class QSettings;
class QSlider;

class DCDReader;

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
        virtual void tick(int framenum, Frame* frame, QuantisedFrame* quantised);

        // epic hack of note
        QVector<QVector<Triangle> > __all__frames__;
        bool __process__frames__(DCDReader* reader, int start, int end);
        bool __save__header__(QDataStream& out);
        bool __save__frames__(QDataStream& out, int start, int end);
        bool __process__and__save__(QString filename, DCDReader* reader);
        bool __process__all__frames__(DCDReader* reader);
        bool __save__all__frames__(QString filename);
        bool __load__all__frames__(QString filename);

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

        int framenum;

        unsigned char*** volumedata;

        QWidget* _preferenceWidget;
        QSlider* metaballsAlphaSlider;
        QSlider* stepSizeSlider;
        QCheckBox* lightCheckBox;
        QCheckBox* cullCheckBox;

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
        bool __do__processing__;

};//MetaballsView

#endif
