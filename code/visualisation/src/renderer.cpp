#include "renderer.h"

#include <cstdio>

#include <QMouseEvent>
#include <QSettings>
#include <QWheelEvent>

#include "base_view.h"
#include "quaternion.h"

#define RAD(x) ((x) * 0.0174532925199433)
#define DEG(x) ((x) * 57.295779513082323)

#define CHECK_GL_ERRORS

Renderer::Renderer(QWidget* parent)
    : QGLWidget(parent)
{
    settings = new QSettings;
    zoom = 10.0f;

    spinning[0] = 0;
    spinning[1] = 0;
    spinning[2] = 0;

    dragging[0] = false;
    dragging[1] = false;
    dragging[2] = false;

    _focusPlane = false;
    focusPlaneDepth = -10;

    _renderMode = -1;

    setMouseTracking(true);
    rot = new Quaternion();
    rot->update_matrix();

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

    setTps(60);
    timer->start();

    scrollSensitivity = 1;

    volume_range[0] = 1;
    volume_range[1] = 1;
    volume_range[2] = 1;
}//constructor

Renderer::~Renderer()
{
    settings->sync();
    delete settings;
    delete timer;
    delete rot;
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
    glDepthFunc(GL_LEQUAL);

    glPointSize(10.0);
    glLineWidth(2.0);
    // glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

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
}//resizeGL

void Renderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (_renderMode < 0)
        return;

    //setup camera
    glTranslatef(0, 0, -zoom);
    glPushMatrix();
    glMultMatrixd(rot->matrix);
    // glTranslatef(-volume_middle[0], -volume_middle[1], -volume_middle[2]);

    // renderAxes();
    renderModes[_renderMode]->render();

    glPopMatrix();
    if (_focusPlane)
    {
        glBegin(GL_QUADS);
        glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
        glVertex3f(-max_side*1, -max_side*1, focusPlaneDepth);
        glVertex3f( max_side*1, -max_side*1, focusPlaneDepth);
        glVertex3f( max_side*1,  max_side*1, focusPlaneDepth);
        glVertex3f(-max_side*1,  max_side*1, focusPlaneDepth);
        glEnd();
    }//if

#ifdef CHECK_GL_ERRORS
    GLenum errcode = glGetError();
    if (errcode != GL_NO_ERROR)
        fprintf(stderr, "GLError: '%s'\n", gluErrorString(errcode));
#endif
}//paintGL

void Renderer::resetView(float volumeSize[3])
{
    rot->reset();

    max_side = 0;
    for (int i = 0; i < 3; i++)
    {
        volume_min[i] = 0;
        volume_max[i] = volumeSize[i];
        volume_range[i] = volume_max[i] - volume_min[i];
        volume_middle[i] = (volume_max[i] + volume_min[i]) * 0.5;
        if (volume_range[i] > max_side)
            max_side = volume_range[i];
    }//for

    //set a default zoom which should cover the entire volume
    zoom = max_side * 1.7;
    focusPlaneDepth = 0;
    scrollSensitivity = max_side * 0.1;
    if (scrollSensitivity < 1) scrollSensitivity = 1;

    //TODO? setup projection matrix so that near and far encompass data?
}//resetView


int Renderer::tps()
{
    return _tps;
}//tps

void Renderer::setTps(int value)
{
    if (value < 0) return;
    _tps = value;
    if (value == 0) timer->stop();
    else timer->setInterval(1000 / _tps);
}//setTps

bool Renderer::focusPlane()
{
    return _focusPlane;
}//focusPlane

void Renderer::toggleFocusPlane()
{
    _focusPlane = !_focusPlane;
}//toggleFocusPlane

int Renderer::renderMode()
{
    return _renderMode;
}//renderMode

BaseView* Renderer::currentView()
{
    if (_renderMode > -1)
        return renderModes[_renderMode];
    return NULL;
}//currentView

void Renderer::setRenderMode(int mode)
{
    if (mode >= renderModes.size())
        mode = renderModes.size() - 1;
    if (mode < -1)
        mode = -1;

    if (_renderMode > -1)
    {
        renderModes[_renderMode]->current = false;
        renderModes[mode]->tick(renderModes[_renderMode]->frame, renderModes[_renderMode]->data);
    }//if
    _renderMode = mode;
    settings->setValue("renderer/renderMode", _renderMode);
    if (_renderMode > -1)
        renderModes[_renderMode]->current = true;
}//setRenderMode

int Renderer::addRenderMode(BaseView* view)
{
    renderModes.push_back(view);
    int viewID = renderModes.size() - 1;
    view->viewID = viewID;
    return viewID;
}//addRenderMode

void Renderer::dataTick(Frame* frame, QuantisedFrame* qframe)
{
    if (_renderMode >= 0)
        renderModes[_renderMode]->tick(frame, qframe);
}//dataTick

void Renderer::tick()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    int difx = lastpos[0] - pos.x();
    int dify = lastpos[1] - pos.y();

    if (dragging[0])
    {
        spinning[0] = RAD(difx);
        spinning[1] = RAD(dify);
    }//if

    if (dragging[1])
    {
        if (_focusPlane)
        {
            focusPlaneDepth -= dify * 2;
        }//if
    }//if

    if (dragging[2])
    {
        spinning[2] = RAD(difx);
    }//if

    //apply the rotations if needed
    if ((difx != 0) || (dify != 0) ||
        (spinning[0] != 0) || (spinning[1] != 0) || (spinning[2] != 0))
    {
        rot->rotate(spinning[0], 0, 1, 0);
        rot->rotate(spinning[1], 1, 0, 0);
        rot->rotate(spinning[2], 0, 0, 1);
        lastpos[0] = pos.x();
        lastpos[1] = pos.y();
        rot->update_matrix();
    }//if

    //change of zoom, rotation, or data update requires an updateGL
    //so instead of checking for all conditions, just updateGL, meh
    updateGL();
}//tick


void Renderer::mouseMoveEvent(QMouseEvent* event)
{
}//mouseMoveEvent

void Renderer::mousePressEvent(QMouseEvent* event)
{
    lastpos[0] = event->x();
    lastpos[1] = event->y();
    switch (event->button())
    {
        case Qt::LeftButton:
            dragging[0] = true;
            break;
        case Qt::MidButton:
            dragging[1] = true;
            break;
        case Qt::RightButton:
            dragging[2] = true;
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
        case Qt::MidButton:
            dragging[1] = false;
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
    int numsteps = event->delta() * scrollSensitivity / (8 * 15);
    zoom -= numsteps;
}//wheelEvent

void Renderer::renderAxes()
{
    //draw axes
    glPushMatrix();
    glTranslatef(volume_min[0], volume_min[1], volume_min[2]);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(volume_range[0], 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, volume_range[1], 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, volume_range[2]);
    glEnd();
    glPopMatrix();
}//renderAxes

