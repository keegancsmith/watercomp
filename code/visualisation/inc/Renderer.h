#ifndef RENDERER_H
#define RENDERER_H

#include <QGLWidget>
#include <QTimer>
#include <QVector>

class QSettings;

class BaseView;
class Quaternion;

class Frame;
class QuantisedFrame;

class Renderer : public QGLWidget
{
    Q_OBJECT
    friend class MainWindow;
    public:
        double scrollSensitivity;

        float volume_min[3];
        float volume_max[3];
        float volume_range[3];
        float volume_middle[3];
        float max_side;

        Renderer(QWidget* parent=0);
        ~Renderer();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        void resetView(float* min_coord, float* max_coord);

        int tps();
        void setTps(int value);
        bool focusPlane();

        float zoom();

        int renderMode();
        BaseView* currentView();
        int addRenderMode(BaseView* view);

    public slots:
        void toggleFocusPlane();
        void setRenderMode(int mode);
        void dataTick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised);

    private slots:
        void tick();

    protected:
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        QSettings* settings;
        float _zoom;
        float spinning[3];
        bool dragging[3];
        int lastpos[2];
        int startdrag[2];
        int framenum;

        bool _focusPlane;
        float focusPlaneDepth;
        int _renderMode;

        int _tps;
        QTimer* timer;

        Quaternion* rot;

        QVector<BaseView*> renderModes;

        void renderAxes();
};//Renderer

#endif
