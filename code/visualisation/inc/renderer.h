#ifndef RENDERER_H
#define RENDERER_H

#include <QGLWidget>
#include <QTimer>

class Frame_Data;
class Quaternion;

class Renderer : public QGLWidget
{
    Q_OBJECT
    friend class MainWindow;
    public:
        const static int RENDER_BLANK = 0;
        const static int RENDER_POINTS = 1;
        const static int RENDER_METABALLS = 2;

        Renderer(QWidget* parent=0);
        ~Renderer();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        void resetView();

        int tps();
        void tps(int value);
        bool focusPlane();
        int renderMode();
        void renderMode(int mode);

    public slots:
        void toggleFocusPlane();

    private slots:
        void tick();

    protected:
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        float zoom;
        float spinning[3];
        bool dragging[3];
        int lastpos[2];
        int startdrag[2];

        bool _focusPlane;
        float focusPlaneDepth;
        int _renderMode;

        int mode;
        float _pointColor[4];
        float _metaballsColor[4];

        int _tps;
        QTimer* timer;

        Quaternion* rot;
        Frame_Data* data;

        void renderAxes();
        void renderPoints();
        void renderMetaballs();
};//Renderer

#endif
