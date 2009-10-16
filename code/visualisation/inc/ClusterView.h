#ifndef CLUSTER_VIEW_H
#define CLUSTER_VIEW_H

#include "BaseView.h"

#include <GL/glu.h>
#include <map>
#include <vector>


class QCheckBox;
class QLabel;
class QSettings;
class QSlider;
class QSpinBox;

class ClusterView : public BaseView
{
    Q_OBJECT

    public:
        ClusterView();
        virtual ~ClusterView();

        virtual void updatePreferences();

        virtual void tick(int framenum, Frame* frame, QuantisedFrame* quantised, Frame* dequantised);
        virtual void render();

    protected:
        virtual void initGL();
        virtual void setupPreferenceWidget(QWidget* preferenceWidget);

    private slots:
        void setLineAlpha(int value);
        void setLineWidth(int value);
        void pickLineColor();
        void setLighting(int state);
        void setClusterID(int value);

    private:
        QSettings* settings;
        float _lineColor[4];
        int _lineWidth;
        int num_clusters;
        int current_cluster;
        GLUquadricObj* quadric;
        bool lighting;

        QSlider* lineWidthSlider;
        QSlider* lineAlphaSlider;
        QSpinBox* clusterSpinBox;
        QCheckBox* lightCheckBox;
        QLabel* countLabel;

        bool first;

        std::map<unsigned int, std::vector<unsigned int> > graph;
        std::map<int, int> components;
        std::map<unsigned int, unsigned int> sizes;


        void dfs(int current, int component);
};//ClusterView

#endif
