#include "renderer.h"

#include <cstdio>

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
    zoom = 10.0f;

    spinning[0] = 0;
    spinning[1] = 0;
    spinning[2] = 0;

    dragging[0] = false;
    dragging[1] = false;
    dragging[2] = false;

    _focusPlane = false;
    focusPlaneDepth = -10;

    _renderMode = RENDER_POINTS;
    _pointColor[0] = 0.0f;
    _pointColor[1] = 0.0f;
    _pointColor[2] = 1.0f;
    _pointColor[3] = 0.02f;

    _metaballsColor[0] = 0.5f;
    _metaballsColor[1] = 0.5f;
    _metaballsColor[2] = 0.5f;
    _metaballsColor[3] = 0.5f;

    setMouseTracking(true);
    rot = new Quaternion();
    rot->update_matrix();

    data = new Frame_Data();

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

    tps(60);
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

    // initMetaballs();
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

    if (_renderMode == RENDER_BLANK)
        return;

    //setup camera
    glTranslatef(0, 0, data->half_size[2]-zoom);
    glPushMatrix();
    glMultMatrixd(rot->matrix);

    switch (_renderMode)
    {
        case RENDER_POINTS:
            renderPoints();
            break;
        case RENDER_METABALLS:
            renderMetaballs();
            break;
    }//switch

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

void Renderer::tps(int value)
{
    if (value < 0) return;
    _tps = value;
    if (value == 0) timer->stop();
    else timer->setInterval(1000 / _tps);
}//tps

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

void Renderer::renderMode(int mode)
{
    _renderMode = mode;
}//tps


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
    switch (_renderMode)
    {
        case RENDER_METABALLS:
            //TODO: get surface if data has changed
            //tickMetaballs();
            break;
    }//switch

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

void Renderer::fillGridCell(GridCell& grid, unsigned char*** data, int i, int x, int y, int z)
{
    grid.p[i].p[0] = x;
    grid.p[i].p[1] = y;
    grid.p[i].p[2] = z;
    grid.val[i] = data[z][y][x];
}//fillGridCell


// marching tetrahedron code implementation by Paul Bourke, url:
// http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
// http://local.wasp.uwa.edu.au/~pbourke/libraries/
void Renderer::marchTetrahedron(QVector<Triangle>& surface, GridCell& g, int iso, int v0, int v1, int v2, int v3)
{
    int triindex = 0;
    // Determine which of the 16 cases we have given which vertices
    // are above or below the isosurface
    if (g.val[v0] < iso) triindex |= 1;
    if (g.val[v1] < iso) triindex |= 2;
    if (g.val[v2] < iso) triindex |= 4;
    if (g.val[v3] < iso) triindex |= 8;

    Triangle t1, t2;
   // Form the vertices of the triangles for each case
   switch (triindex)
   {
       case 0x00:
       case 0x0F:
          break;
       case 0x0E:
       case 0x01:
          t1.p[0] = vertexInterpolate(iso, g, v0, v1);
          t1.p[1] = vertexInterpolate(iso, g, v0, v2);
          t1.p[2] = vertexInterpolate(iso, g, v0, v3);
          surface.push_back(t1);
          break;
       case 0x0D:
       case 0x02:
          t1.p[0] = vertexInterpolate(iso, g, v1, v0);
          t1.p[1] = vertexInterpolate(iso, g, v1, v3);
          t1.p[2] = vertexInterpolate(iso, g, v1, v2);
          surface.push_back(t1);
          break;
       case 0x0C:
       case 0x03:
          t1.p[0] = vertexInterpolate(iso, g, v0, v3);
          t1.p[1] = vertexInterpolate(iso, g, v0, v2);
          t1.p[2] = vertexInterpolate(iso, g, v1, v3);
          surface.push_back(t1);

          t2.p[0] = t1.p[2];
          t2.p[1] = vertexInterpolate(iso, g, v1, v2);
          t2.p[2] = t1.p[1];
          surface.push_back(t2);
          break;
       case 0x0B:
       case 0x04:
          t1.p[0] = vertexInterpolate(iso, g, v2, v0);
          t1.p[1] = vertexInterpolate(iso, g, v2, v1);
          t1.p[2] = vertexInterpolate(iso, g, v2, v3);
          surface.push_back(t1);
          break;
       case 0x0A:
       case 0x05:
          t1.p[0] = vertexInterpolate(iso, g, v0, v1);
          t1.p[1] = vertexInterpolate(iso, g, v2, v3);
          t1.p[2] = vertexInterpolate(iso, g, v0, v3);
          surface.push_back(t1);

          t2.p[0] = t1.p[0];
          t2.p[1] = vertexInterpolate(iso, g, v1, v2);
          t2.p[2] = t1.p[1];
          surface.push_back(t2);
          break;
       case 0x09:
       case 0x06:
          t1.p[0] = vertexInterpolate(iso, g, v0, v1);
          t1.p[1] = vertexInterpolate(iso, g, v1, v3);
          t1.p[2] = vertexInterpolate(iso, g, v2, v3);
          surface.push_back(t1);

          t2.p[0] = t1.p[0];
          t2.p[1] = vertexInterpolate(iso, g, v0, v2);
          t2.p[2] = t1.p[2];
          surface.push_back(t2);
          break;
       case 0x07:
       case 0x08:
          t1.p[0] = vertexInterpolate(iso, g, v3, v0);
          t1.p[1] = vertexInterpolate(iso, g, v3, v2);
          t1.p[2] = vertexInterpolate(iso, g, v3, v1);
          surface.push_back(t1);
          break;
   }//switch
}//marchTetrahedron

Point3f Renderer::vertexInterpolate(float iso, GridCell& g, int v1, int v2)
{
#define ABS(x) (((x) < 0) ? -(x) : (x))
    float d1 = iso - g.val[v1];
    float d2 = iso - g.val[v2];
    float d = g.val[v2] - g.val[v1];
    if (ABS(d1) < 0.00001) return g.p[v1];
    if (ABS(d2) < 0.00001) return g.p[v2];
    if (ABS(d) < 0.00001) return g.p[v1];

   double mu = d1 / d;
   Point3f p;
   for (int i = 0; i < 3; i++)
       p.p[i] = g.p[v1].p[i] + mu * (g.p[v2].p[i] - g.p[v1].p[i]);
   return p;
}//vertexInterpolate

void Renderer::initMetaballs()
{
    unsigned char*** data = NULL;

    int nz = 160;
    int ny = 160;
    int nx = 200;
    int x, y, z;
    data = new unsigned char**[nz];
    for (z = 0; z < nz; z++)
    {
        data[z] = new unsigned char*[ny];
        for (y = 0; y < ny; y++)
            data[z][y] = new unsigned char[nx];
    }//for

    FILE* f = fopen("mri.raw", "rb");
    for (z = 0; z < nz; z++)
        for (y = 0; y < ny; y++)
            for (x = 0; x < nx; x++)
                data[z][y][x] = fgetc(f);
    fclose(f);
    printf("read done\n");

    GridCell grid;
    int n;
    for (z = 0; z < nz/2; z+=1)
    {
        for (y = 0; y < ny/2; y+=1)
        {
            for (x = 0; x < nx/2; x+=1)
            {
                fillGridCell(grid, data, 0, x,   y,   z);
                fillGridCell(grid, data, 1, x+1, y,   z);
                fillGridCell(grid, data, 2, x+1, y+1, z);
                fillGridCell(grid, data, 3, x,   y+1, z);
                fillGridCell(grid, data, 4, x,   y,   z+1);
                fillGridCell(grid, data, 5, x+1, y,   z+1);
                fillGridCell(grid, data, 6, x+1, y+1, z+1);
                fillGridCell(grid, data, 7, x,   y+1, z+1);
                marchTetrahedron(_surface, grid, 128, 0, 2, 3, 7);
                marchTetrahedron(_surface, grid, 128, 0, 2, 6, 7);
                marchTetrahedron(_surface, grid, 128, 0, 4, 6, 7);
                marchTetrahedron(_surface, grid, 128, 0, 6, 1, 2);
                marchTetrahedron(_surface, grid, 128, 0, 6, 1, 4);
                marchTetrahedron(_surface, grid, 128, 5, 6, 1, 4);
            }//for
        }//for
    }//for
    // prints out 3 million...
    printf("surface size: %i\n", _surface.size());

    for (z = 0; z < nz; z++)
    {
        for (y = 0; y < ny; y++)
            delete [] data[z][y];
        delete [] data[z];
    }//for
    delete [] data;
}//initMetaballs

void Renderer::tickMetaballs()
{
}//tickMetaballs

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

void Renderer::renderPoints()
{
    renderAxes();

    //draw points
    glColor4fv(_pointColor);
    glBegin(GL_POINTS);
    for (int i = 0; i < data->natoms(); i++)
    {
        glVertex3dv(data->at(i).pos);
    }//for
    glEnd();
}//renderPoints

void Renderer::renderMetaballs()
{
    renderAxes();

    glColor4fv(_metaballsColor);
    glBegin(GL_TRIANGLES);
    // glVertex3f(0.0f, 0.0f, 0.0f);
    // glVertex3f(1.0f, 0.0f, 0.0f);
    // glVertex3f(1.0f, 1.0f, 0.0f);
    for (int i = 0; i < _surface.size(); i++)
    {
        glVertex3fv(_surface.at(i).p[0].p);
        glVertex3fv(_surface.at(i).p[1].p);
        glVertex3fv(_surface.at(i).p[2].p);
    }//for
    glEnd();
}//renderMetaballs

