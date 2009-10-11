#ifndef CLUSTER_VIEW_H
#define CLUSTER_VIEW_H

#include "BaseView.h"

#include <map>
#include <vector>

#include <splitter/WaterMolecule.h>

class QSettings;
class QSlider;
class QSpinBox;

class ClusterView : public BaseView
{
    Q_OBJECT

    public:
        ClusterView();
        virtual ~ClusterView();

        virtual void init(std::vector<AtomInformation> pdb);

        virtual void updatePreferences();
        virtual QWidget* preferenceWidget();

        virtual void tick(int framenum, Frame* frame, QuantisedFrame* quantised);
        virtual void render();

    protected:
        virtual void initGL();

    private slots:
        void setLineAlpha(int value);
        void setLineWidth(int value);
        void pickLineColor();
        void setClusterID(int value);

    private:
        QSettings* settings;
        float _lineColor[4];
        int _lineWidth;
        int num_clusters;
        int current_cluster;

        QSlider* lineWidthSlider;
        QSlider* lineAlphaSlider;
        QSpinBox* clusterSpinBox;

        std::vector<WaterMolecule> waters;
        std::vector<unsigned int> others;
        std::map<unsigned int, std::vector<unsigned int> > graph;
        std::map<int, int> components;
        std::map<unsigned int, unsigned int> sizes;

        QWidget* _preferenceWidget;

        void setupPreferenceWidget();

        void dfs(int current, int component);
};//ClusterView

#endif
