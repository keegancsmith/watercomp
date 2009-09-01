#include "renderer.h"

#include <stdio.h>
#include <QMouseEvent>
#include <QWheelEvent>

#define CHECK_GL_ERRORS

Renderer::Renderer(QWidget* parent)
    : QGLWidget(parent)
{
    setMouseTracking(true);
    zoom = 6.0f;
    rot[0] = 0;
    rot[1] = 0;
    rot[2] = 0;
    dragging[0] = false;
    dragging[1] = false;
    dragging[2] = false;
}//constructor

Renderer::~Renderer()
{
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
    float far = 100.0f;
    gluPerspective(60.0f, ratio, near, far);
    glMatrixMode(GL_MODELVIEW);

    updateGL();
}//resizeGL

void Renderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -zoom);
    glRotatef(rot[1], 1.0f, 0.0f, 0.0f);
    glRotatef(rot[0], 0.0f, 1.0f, 0.0f);
    // glRotatef(rot[2], 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glEnd();

#ifdef CHECK_GL_ERRORS
    GLenum errcode = glGetError();
    if (errcode != GL_NO_ERROR)
        fprintf(stderr, "GLError: '%s'\n", gluErrorString(errcode));
#endif
}//paintGL

void Renderer::mouseMoveEvent(QMouseEvent* event)
{
    if (dragging[0])
    {
        rot[0] -= lastpos[0] - event->x();
        rot[1] -= lastpos[1] - event->y();
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
    }//switch
}//mousePressEvent

void Renderer::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
        case Qt::LeftButton:
            dragging[0] = false;
            break;
    }//switch
}//mouseReleaseEvent

void Renderer::wheelEvent(QWheelEvent* event)
{
    int numsteps = event->delta() / (8 * 15);
    zoom -= numsteps;
    updateGL();
}//wheelEvent

