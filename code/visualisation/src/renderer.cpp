#include "renderer.h"

#include <cmath>
#include <stdio.h>
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
    setMouseTracking(true);
    zoom = 6.0f;
    // rot[0] = 0;
    // rot[1] = 0;
    // rot[2] = 0;
    rot = new Quaternion();
    rot->update_matrix();
    dragging[0] = false;
    dragging[1] = false;
    dragging[2] = false;

    data = new Frame_Data();
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
    // glEnable(GL_LINE_SMOOTH)
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

    updateGL();
}//resizeGL

void Renderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -zoom);
    glMultMatrixd(rot->matrix);
    // glRotatef(DEG(rot->w), rot->x, rot->y, rot->z);

    // double angle = 2 * acos(rot->w);
    // double s = sqrt(rot->x*rot->x + rot->y*rot->y + rot->z*rot->z);
    // if (s < 0.001) s = 1;
    // s = 1 / s;
    // glRotatef(DEG(angle), rot->x*s, rot->y*s, rot->z*s);

    // glRotatef(rot[1], 1.0f, 0.0f, 0.0f);
    // glRotatef(rot[0], 0.0f, 1.0f, 0.0f);
    // glRotatef(rot[2], 0.0f, 0.0f, 1.0f);

    // {
    // glBegin(GL_QUADS);
    // glColor3f(1.0f, 1.0f, 1.0f);
    // glVertex3f(-1.0f,  1.0f, -1.0f);
    // glVertex3f( 1.0f,  1.0f, -1.0f);
    // glVertex3f( 1.0f,  1.0f,  1.0f);
    // glVertex3f(-1.0f,  1.0f,  1.0f);
    // glColor3f(1.0f, 0.0f, 0.0f);
    // glVertex3f(-1.0f,  1.0f, -1.0f);
    // glVertex3f(-1.0f, -1.0f, -1.0f);
    // glVertex3f( 1.0f, -1.0f, -1.0f);
    // glVertex3f( 1.0f,  1.0f, -1.0f);
    // glColor3f(0.0f, 1.0f, 0.0f);
    // glVertex3f(-1.0f,  1.0f, -1.0f);
    // glVertex3f(-1.0f,  1.0f,  1.0f);
    // glVertex3f(-1.0f, -1.0f,  1.0f);
    // glVertex3f(-1.0f, -1.0f, -1.0f);
    // glColor3f(0.0f, 0.0f, 1.0f);
    // glVertex3f(-1.0f,  1.0f,  1.0f);
    // glVertex3f( 1.0f,  1.0f,  1.0f);
    // glVertex3f( 1.0f, -1.0f,  1.0f);
    // glVertex3f(-1.0f, -1.0f,  1.0f);
    // glColor3f(1.0f, 1.0f, 0.0f);
    // glVertex3f( 1.0f,  1.0f,  1.0f);
    // glVertex3f( 1.0f,  1.0f, -1.0f);
    // glVertex3f( 1.0f, -1.0f, -1.0f);
    // glVertex3f( 1.0f, -1.0f,  1.0f);
    // glColor3f(0.0f, 1.0f, 1.0f);
    // glVertex3f( 1.0f, -1.0f, -1.0f);
    // glVertex3f(-1.0f, -1.0f, -1.0f);
    // glVertex3f(-1.0f, -1.0f,  1.0f);
    // glVertex3f( 1.0f, -1.0f,  1.0f);
    // glEnd();
    // }

    glTranslatef(data->half_size[0], data->half_size[1], data->half_size[2]);

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
    // rot[0] = 0;
    // rot[1] = 0;
    // rot[2] = 0;
    rot->reset();
    double side = data->size[0];
    if (data->size[1] > side)
        side = data->size[1];
    //set a default zoom which should cover the entire volume
    zoom = side;
}//resetView

void Renderer::mouseMoveEvent(QMouseEvent* event)
{
    if (dragging[0])
    {
        // rot[0] -= lastpos[0] - event->x();
        // rot[1] -= lastpos[1] - event->y();
        rot->rotate(RAD(lastpos[0]-event->x()), 0, 1, 0);
        rot->rotate(RAD(lastpos[1]-event->y()), 1, 0, 0);
        rot->update_matrix();
        lastpos[0] = event->x();
        lastpos[1] = event->y();
        updateGL();
    }//if
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
        default:
            break;
    }//switch
}//mouseReleaseEvent

void Renderer::wheelEvent(QWheelEvent* event)
{
    int numsteps = event->delta() / (8 * 15);
    zoom -= numsteps;
    updateGL();
}//wheelEvent

