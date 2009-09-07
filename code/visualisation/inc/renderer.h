#ifndef RENDERER
#define RENDERER

#include <QGLWidget>
#include <QTimer>

class Frame_Data;
class Quaternion;

class Renderer : public QGLWidget
{
    friend class MainWindow;
    public:
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

    protected:
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        float zoom;
        //float rot[3];
        bool dragging[3];
        int lastpos[2];

        int _tps;
        QTimer* timer;

        Quaternion* rot;
        Frame_Data* data;
};//Renderer

#endif