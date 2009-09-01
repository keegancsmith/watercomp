#ifndef RENDERER
#define RENDERER

#include <QGLWidget>

class Renderer : public QGLWidget
{
    public:
        Renderer(QWidget* parent=0);
        ~Renderer();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

    protected:
        void mouseMoveEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private:
        float zoom;
        float rot[3];
        bool dragging[3];
        int lastpos[2];
};//Renderer

#endif
