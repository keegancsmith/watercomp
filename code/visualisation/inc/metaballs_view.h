#ifndef METABALLS_VIEW_H
#define METABALLS_VIEW_H

#include "base_view.h"

#include <QVector>

class QSettings;
class QSlider;

struct Triangle
{
    float p[3][3]; // 3 positions
    float n[3][3]; // 3 normals
};//Triangle


class MetaballsView : public BaseView
{
    Q_OBJECT

    public:
        MetaballsView();
        virtual ~MetaballsView();

        int stepSize();

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void render();
        virtual void tick(Frame_Data* data);

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
        Frame_Data* data;
        float _metaballsColor[4];
        QVector<Triangle> _surface;

        unsigned char*** mridata;

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

};//MetaballsView

#endif
