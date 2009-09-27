#ifndef METABALLS_VIEW_H
#define METABALLS_VIEW_H

#include "base_view.h"

#include <QVector>

class QGridLayout;
class QLabel;
class QPushButton;
class QSlider;

class QCheckBox;


struct Point3f
{
    float p[3];
};//Point3f

struct Triangle
{
    float p[3][3]; // 3 positions
    float n[3][3]; // 3 normals
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
        bool cullFace;
        bool lighting;
        int maxStepSize;
        int _stepSize;
        Frame_Data* data;
        float _metaballsColor[4];
        QVector<Triangle> _surface;


        unsigned char*** mridata;
        QCheckBox** tetrahedrons;

        QWidget* _preferenceWidget;
        QGridLayout* layout;
        QPushButton* metaballsColorButton;
        QLabel* metaballsAlphaLabel;
        QSlider* metaballsAlphaSlider;
        QLabel* stepSizeLabel;
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
