#include "renderer.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include "frame_data.h"
#include "quaternion.h"

#define RAD(x) ((x) * 0.0174532925199433)
#define DEG(x) ((x) * 57.295779513082323)

#define CHECK_GL_ERRORS

Renderer::Renderer(QWidget* parent)
    : QGLWidget(parent)
{
    dragging[0] = false;
    dragging[1] = false;
    dragging[2] = false;

    setMouseTracking(true);
    zoom = 6.0f;
    rot = new Quaternion();
    rot->update_matrix();

    data = new Frame_Data();

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));

    tps(60);
    timer->start();
}//constructor

Renderer::~Renderer()
{
    delete rot;
    delete data;
}//destructor

QSize Renderer::minimumSizeHint() const
{
    return QSize(100, 100);
}//minimumSizeHint

QSize Renderer::sizeHint() const
{
    return QSize(2000, 2000);
}//sizeHint

void Renderer::initializeGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glPointSize(10.0);
    glLineWidth(2.0);
    // glEnable(GL_LINE_SMOOTH);
    // glEnable(GL_POINT_SMOOTH);

    glClearColor(0.0, 0.0, 0.0, 0.0);
}//initializeGL

void Renderer::resizeGL(int w, int h)
{
    if (h < 1) h = 1;
    float ratio = (float) w / (float) h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float near = 0.1f;
    float far = 1000.0f;
    gluPerspective(60.0f, ratio, near, far);
    glMatrixMode(GL_MODELVIEW);

    // updateGL();
}//resizeGL

void Renderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //setup camera
    glTranslatef(-data->half_size[0], -data->half_size[1], -zoom);
    glMultMatrixd(rot->matrix);

    //translate to centroid of points (centre of rotations)
    glTranslatef(data->half_size[0], data->half_size[1], data->half_size[2]);

    //draw axes
    glPushMatrix();
    glTranslatef(data->bbox[0][0], data->bbox[0][1], data->bbox[0][2]);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(data->size[0], 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, data->size[1], 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, data->size[2]);
    glEnd();
    glPopMatrix();

    //draw points
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < data->natoms(); i++)
    {
        glVertex3dv(data->at(i).pos);
    }//for
    glEnd();

#ifdef CHECK_GL_ERRORS
    GLenum errcode = glGetError();
    if (errcode != GL_NO_ERROR)
        fprintf(stderr, "GLError: '%s'\n", gluErrorString(errcode));
#endif
}//paintGL

void Renderer::resetView()
{
    rot->reset();
    double side = data->size[0];
    if (data->size[1] > side)
        side = data->size[1];
    //set a default zoom which should cover the entire volume
    zoom = side + data->size[2];

    //TODO? setup projection matrix so that near and far encompass data?
}//resetView


int Renderer::tps()
{
    return _tps;
}//tps

void Renderer::tps(int value)
{
    if (value < 0) return;
    _tps = value;
    if (value == 0) timer->stop();
    else timer->setInterval(1000 / _tps);
}//tps


void Renderer::mouseMoveEvent(QMouseEvent* event)
{
    //left drag to rotate
    if (dragging[0])
    {
        rot->rotate(RAD(lastpos[0]-event->x()), 0, 1, 0);
        rot->rotate(RAD(lastpos[1]-event->y()), 1, 0, 0);
        rot->update_matrix();

        lastpos[0] = event->x();
        lastpos[1] = event->y();
    }//if
    else if (dragging[2])
    {
        rot->rotate(RAD(lastpos[0]-event->x()), 0, 0, 1);
        //rot->rotate(-RAD(lastpos[1]-event->y()), 0, 0, 1);
        rot->update_matrix();

        lastpos[0] = event->x();
        lastpos[1] = event->y();
    }//else
}//mouseMoveEvent

void Renderer::mousePressEvent(QMouseEvent* event)
{
    switch (event->button())
    {
        case Qt::LeftButton:
            dragging[0] = true;
            lastpos[0] = event->x();
            lastpos[1] = event->y();
            break;
        case Qt::RightButton:
            dragging[2] = true;
            lastpos[0] = event->x();
            lastpos[1] = event->y();
            break;
        default:
            break;
    }//switch
}//mousePressEvent

void Renderer::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
        case Qt::LeftButton:
            dragging[0] = false;
            break;
        case Qt::RightButton:
            dragging[2] = false;
            break;
        default:
            break;
    }//switch
}//mouseReleaseEvent

void Renderer::wheelEvent(QWheelEvent* event)
{
    int numsteps = event->delta() / (8 * 15);
    zoom -= numsteps;
    // updateGL();
}//wheelEvent

