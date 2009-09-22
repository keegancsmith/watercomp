#include "renderer.h"

#include <cstdio>

#include <QMouseEvent>
#include <QWheelEvent>

#include "base_view.h"
#include "frame_data.h"
#include "quaternion.h"

#define RAD(x) ((x) * 0.0174532925199433)
#define DEG(x) ((x) * 57.295779513082323)

#define CHECK_GL_ERRORS

Renderer::Renderer(QWidget* parent)
    : QGLWidget(parent)
{
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

    data = new Frame_Data();

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

    setTps(60);
    timer->start();
}//constructor

Renderer::~Renderer()
{
    delete timer;
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
    glDepthFunc(GL_ALWAYS);

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
    glTranslatef(0, 0, data->half_size[2]-zoom);
    glPushMatrix();
    glMultMatrixd(rot->matrix);

    renderAxes();
    renderModes[_renderMode]->render();
    // switch (_renderMode)
    // {
        // case RENDER_POINTS:
            // renderPoints();
            // break;
        // case RENDER_METABALLS:
            // renderMetaballs();
            // break;
    // }//switch

    glPopMatrix();
    if (_focusPlane)
    {
        glDepthFunc(GL_LEQUAL);
        glBegin(GL_QUADS);
        glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
        glVertex3f(-data->max_side*1, -data->max_side*1, focusPlaneDepth);
        glVertex3f( data->max_side*1, -data->max_side*1, focusPlaneDepth);
        glVertex3f( data->max_side*1,  data->max_side*1, focusPlaneDepth);
        glVertex3f(-data->max_side*1,  data->max_side*1, focusPlaneDepth);
        glEnd();
        glDepthFunc(GL_ALWAYS);
    }//if

#ifdef CHECK_GL_ERRORS
    GLenum errcode = glGetError();
    if (errcode != GL_NO_ERROR)
        fprintf(stderr, "GLError: '%s'\n", gluErrorString(errcode));
#endif
}//paintGL

void Renderer::resetView()
{
    rot->reset();
    //set a default zoom which should cover the entire volume
    zoom = data->max_side * 1.7;
    // focusPlaneDepth = -data->max_side;
    focusPlaneDepth = 0;

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
    return renderModes[_renderMode];
}//currentView

void Renderer::setRenderMode(int mode)
{
    if (mode >= renderModes.size())
        mode = renderModes.size() - 1;
    if (mode < -1)
        mode = -1;
    _renderMode = mode;
}//setRenderMode

int Renderer::addRenderMode(BaseView* view)
{
    renderModes.push_back(view);
    int viewID = renderModes.size() - 1;
    view->viewID = viewID;
    return viewID;
}//addRenderMode

void Renderer::dataTick()
{
    if (_renderMode >= 0)
        renderModes[_renderMode]->tick(data);
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

    //depending on _renderMode, may need to do processing
    // if (_renderMode >= 0)
    // {
        // renderModes[_renderMode]->tick(data);
    // }//if

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
    double sensitivity = data->max_side * 0.1;
    if (sensitivity < 1) sensitivity = 1;
    int numsteps = event->delta() * sensitivity / (8 * 15);
    zoom -= numsteps;
}//wheelEvent

void Renderer::renderAxes()
{
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
}//renderAxes

