#ifndef METABALLS_VIEW_H
#define METABALLS_VIEW_H

#include "BaseView.h"

#include <gts.h>
#include <map>
#include <QHash>
#include <QVector>

#define USE_GRID

class QCheckBox;
class QFile;
class QDataStream;
class QSettings;
class QSlider;
class QSpinBox;

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

        virtual void tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised);
        virtual void render();

        bool processVolume(QString filename, DCDReader* reader, int quant_level=8);
        bool loadFile(QString filename);

    protected:
        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

    private slots:
        void setMetaballsAlpha(int value);
        void setStepSize(int value);
        void pickMetaballsColor();

        void setCullFace(int state);
        void setLighting(int state);
        void setUseDataFile(int state);
        void setIsoValue(int value);

        void updateFaces();

    private:
        QSettings* settings;
        bool cullFace;
        bool lighting;
        int maxStepSize;
        int _stepSize;
        float _metaballsColor[4];
        QVector<Triangle> _surface;
        bool haveDataFile;
        bool useDataFile;
        int isoValue;

        int max_quant;
        int cur_quant;
        int size;

#ifdef USE_GRID
        unsigned char*** volumedata;
#else
        QHash<Point3f, int> meta_volume;
#endif

        QSlider* metaballsAlphaSlider;
        QSpinBox* stepSizeSpinBox;
        QCheckBox* lightCheckBox;
        QCheckBox* cullCheckBox;
        QCheckBox* dataFileCheckBox;
        QSpinBox* isoValueSpinBox;


        float sampleVolume(float x, float y, float z);
        void getNormal(float* normal, float x, float y, float z);
        float getOffset(float f1, float f2, float value);
        void doMarchingTetrahedron(float tetrahedronPosition[4][3], float tetrahedronValue[3]);
        void callMarchingTetrahedrons(float x, float y, float z, float scale);
        void callMarchingCubes(float x, float y, float z, float scale);

        GtsSurface* g_surface;
        GtsCartesianGrid g_grid;


        QFile* meta_file;
        QDataStream* meta_data;
        QVector<qint64> meta_pos;
        int lastframe;
        int loadedframe;

};//MetaballsView

#endif
