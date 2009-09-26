#include "metaballs_view.h"

#include <GL/gl.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

#include "frame_data.h"

#define ALPHA_MAX_SLIDER 100
#define ALPHA_MAX_VAL 1.0

#define STEP_SIZE 10

float* pack3f(float* v, float a, float b, float c)
{
    v[0] = a;
    v[1] = b;
    v[2] = c;
    return v;
}//pack3f

float* sub2v(float* v1, float* v2, float* dst)
{
    dst[0] = v1[0] - v2[0];
    dst[1] = v1[1] - v2[1];
    dst[2] = v1[2] - v2[2];
    return dst;
}//sub

float* cross(float* v1, float* v2, float* dst)
{
    float t0 = v1[1]*v2[2] - v1[2]*v2[1];
    float t1 = v1[2]*v2[0] - v1[0]*v2[2];
    float t2 = v1[0]*v2[1] - v1[1]*v2[0];
    return pack3f(dst, t0, t1, t2);
}//cross


MetaballsView::MetaballsView()
{
    viewName = "Metaballs view";
    _metaballsColor[0] = 0.5f;
    _metaballsColor[1] = 0.5f;
    _metaballsColor[2] = 0.5f;
    _metaballsColor[3] = 0.5f;
    data = 0;

    _preferenceWidget = NULL;
    init();
}//constructor

MetaballsView::~MetaballsView()
{
}//destructor


void MetaballsView::init()
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
    for (z = 0; z < nz-STEP_SIZE; z+=STEP_SIZE)
    {
        for (y = 0; y < ny-STEP_SIZE; y+=STEP_SIZE)
        {
            for (x = 0; x < nx-STEP_SIZE; x+=STEP_SIZE)
            {
                fillGridCell(grid, data, 0, x, y, z);
                fillGridCell(grid, data, 1, x+STEP_SIZE, y, z);
                fillGridCell(grid, data, 2, x+STEP_SIZE, y+STEP_SIZE, z);
                fillGridCell(grid, data, 3, x, y+STEP_SIZE, z);
                fillGridCell(grid, data, 4, x, y, z+STEP_SIZE);
                fillGridCell(grid, data, 5, x+STEP_SIZE, y, z+STEP_SIZE);
                fillGridCell(grid, data, 6, x+STEP_SIZE, y+STEP_SIZE, z+STEP_SIZE);
                fillGridCell(grid, data, 7, x, y+STEP_SIZE, z+STEP_SIZE);
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
}//init


void MetaballsView::updatePreferences()
{
    metaballsAlphaSlider->setValue((int)(_metaballsColor[3] * ALPHA_MAX_SLIDER / ALPHA_MAX_VAL));
}//updatePreferences

QWidget* MetaballsView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
        setupPreferenceWidget();
    return _preferenceWidget;
}//preferenceWidget

void MetaballsView::render()
{
    glTranslatef(-80, -80, -100);
    glColor4fv(_metaballsColor);
    glBegin(GL_TRIANGLES);
    // glVertex3f(0.0f, 0.0f, 0.0f);
    // glVertex3f(1.0f, 0.0f, 0.0f);
    // glVertex3f(1.0f, 1.0f, 0.0f);
    for (int i = 0; i < _surface.size(); i++)
    {
        glNormal3fv(_surface.at(i).n[0].p);
        glVertex3fv(_surface.at(i).p[0].p);
        glVertex3fv(_surface.at(i).p[1].p);
        glVertex3fv(_surface.at(i).p[2].p);
    }//for
    glEnd();
}//render

void MetaballsView::tick(Frame_Data* data)
{
    this->data = data;
}//tick


void MetaballsView::setMetaballsAlpha(int value)
{
    if (value > ALPHA_MAX_SLIDER) value = ALPHA_MAX_SLIDER;
    if (value < 0) value = 0;
    _metaballsColor[3] = (float)value * ALPHA_MAX_VAL / ALPHA_MAX_SLIDER;
}//setMetaballsAlpha

void MetaballsView::pickMetaballsColor()
{
    pickColor(_metaballsColor);
}//pickMetaballsColor


void MetaballsView::fillGridCell(GridCell& grid, unsigned char*** data, int i, int x, int y, int z)
{
    grid.p[i].p[0] = x;
    grid.p[i].p[1] = y;
    grid.p[i].p[2] = z;
    grid.val[i] = data[z][y][x];
}//fillGridCell

void MetaballsView::addTriangle(QVector<Triangle>& surface, GridCell& g, int iso, int v0, int v1, int v2, int v3, int v4, int v5)
{
    Triangle t1;
    float side1[3];
    float side2[3];

    t1.p[0] = vertexInterpolate(iso, g, v0, v1);
    t1.p[1] = vertexInterpolate(iso, g, v2, v3);
    t1.p[2] = vertexInterpolate(iso, g, v4, v5);
    sub2v(t1.p[1].p, t1.p[0].p, side1);
    sub2v(t1.p[2].p, t1.p[0].p, side2);
    cross(side1, side2, t1.n[0].p);
    surface.push_back(t1);
}//addTriangle

// marching tetrahedron code implementation by Paul Bourke, url:
// http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
// http://local.wasp.uwa.edu.au/~pbourke/libraries/
void MetaballsView::marchTetrahedron(QVector<Triangle>& surface, GridCell& g, int iso, int v0, int v1, int v2, int v3)
{
    int triindex = 0;
    // Determine which of the 16 cases we have given which vertices
    // are above or below the isosurface
    if (g.val[v0] < iso) triindex |= 1;
    if (g.val[v1] < iso) triindex |= 2;
    if (g.val[v2] < iso) triindex |= 4;
    if (g.val[v3] < iso) triindex |= 8;

    // Form the vertices of the triangles for each case
    switch (triindex)
    {
        case 0x00:
        case 0x0F:
            break;

        case 0x0E:
        case 0x01:
            addTriangle(surface, g, iso, v0, v1, v0, v2, v0, v3);
            break;

        case 0x0D:
        case 0x02:
            addTriangle(surface, g, iso, v1, v0, v1, v3, v1, v2);
            break;

        case 0x0C:
        case 0x03:
            addTriangle(surface, g, iso, v0, v3, v0, v2, v1, v3);
            addTriangle(surface, g, iso, v1, v3, v1, v2, v0, v2);
            break;

        case 0x0B:
        case 0x04:
            addTriangle(surface, g, iso, v2, v0, v2, v1, v2, v3);
            break;

        case 0x0A:
        case 0x05:
            addTriangle(surface, g, iso, v0, v1, v2, v3, v0, v3);
            addTriangle(surface, g, iso, v0, v1, v1, v2, v2, v3);
            break;

        case 0x09:
        case 0x06:
            addTriangle(surface, g, iso, v0, v1, v1, v3, v2, v3);
            addTriangle(surface, g, iso, v0, v1, v0, v2, v2, v3);
            break;

        case 0x07:
        case 0x08:
            addTriangle(surface, g, iso, v3, v0, v3, v2, v3, v1);
            break;
   }//switch
}//marchTetrahedron

Point3f MetaballsView::vertexInterpolate(float iso, GridCell& g, int v1, int v2)
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


void MetaballsView::setupPreferenceWidget()
{
    _preferenceWidget = new QWidget;

    layout = new QGridLayout(_preferenceWidget);

    metaballsColorButton = new QPushButton(tr("Select metaballs colour"), _preferenceWidget);
    connect(metaballsColorButton, SIGNAL(clicked()), this, SLOT(pickMetaballsColor()));
    layout->addWidget(metaballsColorButton, 0, 0, 1, 2);

    metaballsAlphaLabel = new QLabel(tr("Metaballs alpha"), _preferenceWidget);
    layout->addWidget(metaballsAlphaLabel, 1, 0);

    metaballsAlphaSlider = new QSlider(_preferenceWidget);
    metaballsAlphaSlider->setOrientation(Qt::Horizontal);
    metaballsAlphaSlider->setRange(0, ALPHA_MAX_SLIDER);
    connect(metaballsAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setMetaballsAlpha(int)));
    layout->addWidget(metaballsAlphaSlider, 1, 1);

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

