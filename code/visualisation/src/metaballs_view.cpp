#include "metaballs_view.h"

#include <GL/gl.h>
#include <cmath>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

#include <QCheckBox>

#include "frame_data.h"

#define ALPHA_MAX_SLIDER 100
#define ALPHA_MAX_VAL 1.0


#include "marching_tables.cpp"


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

float* multf(float* v, float f)
{
    v[0] *= f;
    v[1] *= f;
    v[2] *= f;
    return v;
}//mult

float len2(float* v)
{
    return (float) (v[0] * v[0] +
                    v[1] * v[1] +
                    v[2] * v[2]);
}//len2

float* normalize(float* v)
{
    float f = len2(v);
    if (f == 0)
        return v;
    if ((f < 0.99f) || (f > 1.01f))
        multf(v, 1/sqrt(f));
    return v;
}//normalize



MetaballsView::MetaballsView()
{
    // TODO: define maxStepSize relative to the volume size
    // TODO: define _stepSize relative to the volume size
    maxStepSize = 80;
    _stepSize = 5;

    viewName = "Metaballs view";
    _metaballsColor[0] = 0.5f;
    _metaballsColor[1] = 0.5f;
    _metaballsColor[2] = 0.5f;
    _metaballsColor[3] = 1.0f;
    data = 0;

    lighting = false;
    cullFace = true;

    _preferenceWidget = NULL;
    setupPreferenceWidget();
    init();
}//constructor

MetaballsView::~MetaballsView()
{
    int nz = 160;
    int ny = 160;
    int nx = 200;
    int x, y, z;
    for (z = 0; z < nz; z++)
    {
        for (y = 0; y < ny; y++)
            delete [] mridata[z][y];
        delete [] mridata[z];
    }//for
    delete [] mridata;
}//destructor


int MetaballsView::stepSize()
{
    return _stepSize;
}//stepSize


void MetaballsView::init()
{

    int nz = 160;
    int ny = 160;
    int nx = 200;
    int x, y, z;
    mridata = new unsigned char**[nz];
    for (z = 0; z < nz; z++)
    {
        mridata[z] = new unsigned char*[ny];
        for (y = 0; y < ny; y++)
            mridata[z][y] = new unsigned char[nx];
    }//for


    // FILE* f = fopen("mri.raw", "rb");
    // for (z = 0; z < nz; z++)
        // for (y = 0; y < ny; y++)
            // for (x = 0; x < nx; x++)
                // mridata[z][y][x] = fgetc(f);
    // fclose(f);
    // printf("read done\n");

    int dz, dy, dx, dd;
    float r2 = 300.0 * 255;
    for (z = 0; z < nz; z++)
    {
        dz = z - 80;
        dz *= dz;
        for (y = 0; y < ny; y++)
        {
            dy = y - 80;
            dy *= dy;
            for (x = 0; x < nx; x++)
            {
                dx = x - 100;
                dx *= dx;

                dd = dx + dy + dz;
                mridata[z][y][x] = (dd <= 0.001) ? 255 : r2 / dd;
            }//for
        }//for
    }//for

}//init


void MetaballsView::updatePreferences()
{
    metaballsAlphaSlider->setValue((int)(_metaballsColor[3] * ALPHA_MAX_SLIDER / ALPHA_MAX_VAL));
    stepSizeSlider->setValue(_stepSize);
}//updatePreferences

QWidget* MetaballsView::preferenceWidget()
{
    if (_preferenceWidget == NULL)
        setupPreferenceWidget();
    return _preferenceWidget;
}//preferenceWidget

void MetaballsView::render()
{
    float l_pos[] = {200.0f, 200.0f, 200.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);

    glTranslatef(-80, -80, -100);
    glColor4fv(_metaballsColor);
    glBegin(GL_TRIANGLES);
    // glBegin(GL_LINE_LOOP);
    // glVertex3f(0.0f, 0.0f, 0.0f);
    // glVertex3f(1.0f, 0.0f, 0.0f);
    // glVertex3f(1.0f, 1.0f, 0.0f);
    Triangle t;
    for (int i = 0; i < _surface.size(); i++)
    {
        t = _surface.at(i);
        glNormal3fv(t.n[0]);
        glVertex3fv(t.p[0]);
        glNormal3fv(t.n[1]);
        glVertex3fv(t.p[1]);
        glNormal3fv(t.n[2]);
        glVertex3fv(t.p[2]);
    }//for
    glEnd();
}//render

void MetaballsView::tick(Frame_Data* data)
{
    this->data = data;

    int nz = 160 - _stepSize - 1;
    int ny = 160 - _stepSize - 1;
    int nx = 200 - _stepSize - 1;
    int x, y, z;
    _surface.clear();
    for (z = 0; z < nz; z+=_stepSize)
    {
        for (y = 0; y < ny; y+=_stepSize)
        {
            for (x = 0; x < nx; x+=_stepSize)
            {
                // callMarchingTetrahedrons(x, y, z, _stepSize);
                callMarchingCubes(x, y, z, _stepSize);
            }//for
        }//for
    }//for
    // prints out 3 million...
    printf("surface size: %i\n", _surface.size());

    Triangle t;
    int v;
    float dx, dy, dz, dd;
    float min, max;
    float maxv[3];

    t = _surface.at(0);
    maxv[0] = t.p[0][0];
    maxv[1] = t.p[0][1];
    maxv[2] = t.p[0][2];
    dx = maxv[0] - 100;
    dy = maxv[1] - 80;
    dz = maxv[2] - 80;
    dd = dx*dx + dy*dy + dz*dz;
    min = dd;
    max = dd;

    for (int i = 0; i < _surface.size(); i++)
    {
        t = _surface.at(i);
        for (v = 0; v < 3; v++)
        {
            // if (t->p[v][0] < 5)
                // printf("wtf?\n");

            dx = t.p[v][0] - 100;
            dy = t.p[v][1] - 80;
            dz = t.p[v][2] - 80;
            dd = dx*dx + dy*dy + dz*dz;

            if (dd < min) min = dd;
            if (dd > max)
            {
                max = dd;
                maxv[0] = t.p[v][0];
                maxv[1] = t.p[v][1];
                maxv[2] = t.p[v][2];
            }//if
        }//for
    }//for
    printf("min: %f\nmax: %f (%f, %f, %f)\n", min, max, maxv[0], maxv[1], maxv[2]);

}//tick


void MetaballsView::initGL()
{
    // float m_amb[] = {0.3f, 0.3f, 0.3f, 1.0f};
    // float m_spe[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // float m_shin[] = {50.0f};
    // float l_pos[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float l_pos[] = {200.0f, 200.0f, 200.0f, 0.0f};
    // float l_amb[] = {0.4f, 0.4f, 0.4f, 1.0f};
    // float l_dif[] = {0.7f, 0.7f, 0.7f, 1.0f};
    // float l_spe[] = {0.9f, 0.9f, 0.9f, 1.0f};

    // glEnable(GL_LIGHT0);
    // glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m_amb);
    // glMaterialfv(GL_FRONT, GL_SPECULAR, m_spe);
    // glMaterialfv(GL_FRONT, GL_SHININESS, m_shin);

    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
    // glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, l_spe);


    // float g_amb[] = {1.0f, 1.0f, 1.0f, 1.0f};
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_amb);

    float afPropertiesAmbient [] = {0.50, 0.50, 0.50, 1.00};
    float afPropertiesDiffuse [] = {0.75, 0.75, 0.75, 1.00};
    float afPropertiesSpecular[] = {1.00, 1.00, 1.00, 1.00};
    float afAmbientWhite [] = {0.25, 0.25, 0.25, 1.00};
    float afAmbientRed   [] = {0.25, 0.00, 0.00, 1.00};
    float afAmbientGreen [] = {0.00, 0.25, 0.00, 1.00};
    float afAmbientBlue  [] = {0.00, 0.00, 0.25, 1.00};
    float afDiffuseWhite [] = {0.75, 0.75, 0.75, 1.00};
    float afDiffuseRed   [] = {0.75, 0.00, 0.00, 1.00};
    float afDiffuseGreen [] = {0.00, 0.75, 0.00, 1.00};
    float afDiffuseBlue  [] = {0.00, 0.00, 0.75, 1.00};
    float afSpecularWhite[] = {1.00, 1.00, 1.00, 1.00};
    float afSpecularRed  [] = {1.00, 0.25, 0.25, 1.00};
    float afSpecularGreen[] = {0.25, 1.00, 0.25, 1.00};
    float afSpecularBlue [] = {0.25, 0.25, 1.00, 1.00};
    glLightfv( GL_LIGHT0, GL_AMBIENT,  afPropertiesAmbient);
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  afPropertiesDiffuse);
    glLightfv( GL_LIGHT0, GL_SPECULAR, afPropertiesSpecular);
    glMaterialfv(GL_BACK,  GL_AMBIENT,   afAmbientGreen);
    glMaterialfv(GL_BACK,  GL_DIFFUSE,   afDiffuseGreen);
    glMaterialfv(GL_FRONT, GL_AMBIENT,   afAmbientBlue);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   afDiffuseBlue);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  afSpecularWhite);
    glMaterialf( GL_FRONT, GL_SHININESS, 15.0);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    setLighting(lighting);
    setCullFace(cullFace);

    glDepthFunc(GL_LEQUAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}//initGL


void MetaballsView::setMetaballsAlpha(int value)
{
    if (value > ALPHA_MAX_SLIDER) value = ALPHA_MAX_SLIDER;
    if (value < 0) value = 0;
    _metaballsColor[3] = (float)value * ALPHA_MAX_VAL / ALPHA_MAX_SLIDER;
}//setMetaballsAlpha

void MetaballsView::setStepSize(int value)
{
    _stepSize = value;
}//setStepSize

void MetaballsView::pickMetaballsColor()
{
    pickColor(_metaballsColor);
}//pickMetaballsColor


void MetaballsView::setCullFace(int state)
{
    if (state == 0)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
}//setCullFace

void MetaballsView::setLighting(int state)
{
    if (state == 0)
        glDisable(GL_LIGHTING);
    else
        glEnable(GL_LIGHTING);
}//setLighting

void MetaballsView::updateFaces()
{
    tick(0);
}//updateFaces


float MetaballsView::sampleVolume(float x, float y, float z)
{
    int ix = (int)x;
    int iy = (int)y;
    int iz = (int)z;
    return mridata[iz][iy][ix];
}//sampleVolume


//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
void MetaballsView::getNormal(float* normal, float x, float y, float z)
{
    normal[0] = sampleVolume(x-1, y, z) - sampleVolume(x+1, y, z);
    normal[0] = sampleVolume(x, y-1, z) - sampleVolume(x, y+1, z);
    normal[0] = sampleVolume(x, y, z-1) - sampleVolume(x, y, z+1);
    normalize(normal);
}

//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
float MetaballsView::getOffset(float f1, float f2, float value)
{
    float delta = f2 - f1;

    if (delta == 0.0)
        return 0.5;

    return (value - f1)/delta;
}


//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
void MetaballsView::doMarchingTetrahedron(float tetrahedronPosition[4][3], float tetrahedronValue[3])
{
    float threshold = 128;

    int vertex;
    int flagIndex = 0;
    //Find which vertices are inside of the surface and which are outside
    for(vertex = 0; vertex < 4; vertex++)
    {
        if(tetrahedronValue[vertex] <= threshold)
            flagIndex |= 1 << vertex;
    }

    //Find which edges are intersected by the surface
    int edgeFlags = tetrahedronEdgeFlags[flagIndex];

    //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
    if(edgeFlags == 0)
    {
        return;
    }

    int vert0, vert1;
    float offset, invOffset;
    float edgeVertex[6][3];
    float edgeNormal[6][3];
    //Find the point of intersection of the surface with each edge
    // Then find the normal to the surface at those points
    for(int edge = 0; edge < 6; edge++)
    {
        //if there is an intersection on this edge
        if(edgeFlags & (1 << edge))
        {
            vert0 = tetrahedronEdgeConnection[edge][0];
            vert1 = tetrahedronEdgeConnection[edge][1];
            offset = getOffset(tetrahedronValue[vert0], tetrahedronValue[vert1], threshold);
            invOffset = 1.0 - offset;

            edgeVertex[edge][0] = invOffset*tetrahedronPosition[vert0][0]  +  offset*tetrahedronPosition[vert1][0];
            edgeVertex[edge][1] = invOffset*tetrahedronPosition[vert0][1]  +  offset*tetrahedronPosition[vert1][1];
            edgeVertex[edge][2] = invOffset*tetrahedronPosition[vert0][2]  +  offset*tetrahedronPosition[vert1][2];

            getNormal(edgeNormal[edge], edgeVertex[edge][0], edgeVertex[edge][1], edgeVertex[edge][2]);
        }
    }

    int corner;
    //Draw the triangles that were found.  There can be up to 2 per tetrahedron
    for(int triangle = 0; triangle < 2; triangle++)
    {
        if(tetrahedronTriangles[flagIndex][3*triangle] < 0)
            break;

        for(corner = 0; corner < 3; corner++)
        {
            vertex = tetrahedronTriangles[flagIndex][3*triangle+corner];

            Triangle t;
            t.p[corner][0] = edgeVertex[vertex][0];
            t.p[corner][1] = edgeVertex[vertex][1];
            t.p[corner][2] = edgeVertex[vertex][2];
            t.n[corner][0] = edgeNormal[vertex][0];
            t.n[corner][1] = edgeNormal[vertex][1];
            t.n[corner][2] = edgeNormal[vertex][2];
            _surface.push_back(t);
        }
    }
}

//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
void MetaballsView::callMarchingTetrahedrons(float x, float y, float z, float scale)
{
    int vertex;
    float cubePosition[8][3];
    float cubeValue[8];

    //Make a local copy of the cube's corner positions
    for(vertex = 0; vertex < 8; vertex++)
    {
        cubePosition[vertex][0] = x + vertexOffset[vertex][0]*scale;
        cubePosition[vertex][1] = y + vertexOffset[vertex][1]*scale;
        cubePosition[vertex][2] = z + vertexOffset[vertex][2]*scale;
    }

    //Make a local copy of the cube's corner values
    for(vertex = 0; vertex < 8; vertex++)
    {
        cubeValue[vertex] = sampleVolume(cubePosition[vertex][0],cubePosition[vertex][1], cubePosition[vertex][2]);
        // cubeValue[v] = mridata[cubePosition[v][2]][cubePosition[v][1]][cubePosition[v][0]];
    }

    int i;
    float tetrahedronPosition[4][3];
    float tetrahedronValue[4];
    //for each tetrahedron
    for(int tetrahedron = 0; tetrahedron < 6; tetrahedron++)
    {
        //get the tetrahedron position and value
        for(vertex = 0; vertex < 4; vertex++)
        {
            i = tetrahedronsInACube[tetrahedron][vertex];
            tetrahedronPosition[vertex][0] = cubePosition[i][0];
            tetrahedronPosition[vertex][1] = cubePosition[i][1];
            tetrahedronPosition[vertex][2] = cubePosition[i][2];
            tetrahedronValue[vertex] = cubeValue[i];
        }
        doMarchingTetrahedron(tetrahedronPosition, tetrahedronValue);
    }
}


void MetaballsView::callMarchingCubes(float x, float y, float z, float scale)
{
    float threshold = 128;

    int vertex;
    float cubeValue[8];
    //Make a local copy of the values at the cube's corners
    for(vertex = 0; vertex < 8; vertex++)
    {
        cubeValue[vertex] = sampleVolume(
                x + vertexOffset[vertex][0]*scale,
                y + vertexOffset[vertex][1]*scale,
                z + vertexOffset[vertex][2]*scale);
    }

    //Find which vertices are inside of the surface and which are outside
    int flagIndex = 0;
    for(vertex = 0; vertex < 8; vertex++)
    {
        if(cubeValue[vertex] <= threshold)
            flagIndex |= 1 << vertex;
    }

    //Find which edges are intersected by the surface
    int edgeFlags = cubeEdgeFlags[flagIndex];

    //If the cube is entirely inside or outside of the surface, then there will be no intersections
    if(edgeFlags == 0)
    {
        return;
    }

    float offset;
    float edgeVertex[12][3];
    float edgeNormal[12][3];
    //Find the point of intersection of the surface with each edge
    //Then find the normal to the surface at those points
    for(int edge = 0; edge < 12; edge++)
    {
        //if there is an intersection on this edge
        if(edgeFlags & (1<<edge))
        {
            offset = getOffset(cubeValue[edgeConnection[edge][0]],
                    cubeValue[edgeConnection[edge][1]], threshold);

            edgeVertex[edge][0] = x + (vertexOffset[edgeConnection[edge][0]][0]  +  offset * edgeDirection[edge][0]) * scale;
            edgeVertex[edge][1] = y + (vertexOffset[edgeConnection[edge][0]][1]  +  offset * edgeDirection[edge][1]) * scale;
            edgeVertex[edge][2] = z + (vertexOffset[edgeConnection[edge][0]][2]  +  offset * edgeDirection[edge][2]) * scale;

            getNormal(edgeNormal[edge], edgeVertex[edge][0], edgeVertex[edge][1], edgeVertex[edge][2]);
        }
    }

    int corner;
    //Draw the triangles that were found.  There can be up to five per cube
    for(int triangle = 0; triangle < 5; triangle++)
    {
        if(triangleConnectionTable[flagIndex][3*triangle] < 0)
            break;

        for(corner = 0; corner < 3; corner++)
        {
            vertex = triangleConnectionTable[flagIndex][3*triangle+corner];

            Triangle t;
            t.p[corner][0] = edgeVertex[vertex][0];
            t.p[corner][1] = edgeVertex[vertex][1];
            t.p[corner][2] = edgeVertex[vertex][2];

            t.n[corner][0] = edgeNormal[vertex][0];
            t.n[corner][1] = edgeNormal[vertex][1];
            t.n[corner][2] = edgeNormal[vertex][2];

            if (t.p[corner][0] < 5)
                printf("w t f\n");
            _surface.push_back(t);
            // printf("%f %f %f\n", edgeVertex[vertex][0], edgeVertex[vertex][1], edgeVertex[vertex][2]);
        }
    }
}//callMarchingCubes



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

    stepSizeLabel = new QLabel(tr("Grid size"), _preferenceWidget);
    layout->addWidget(stepSizeLabel, 2, 0);

    stepSizeSlider = new QSlider(_preferenceWidget);
    stepSizeSlider->setOrientation(Qt::Horizontal);
    stepSizeSlider->setRange(0, maxStepSize);
    connect(stepSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setStepSize(int)));
    layout->addWidget(stepSizeSlider, 2, 1);

    QCheckBox* cullCheckBox = new QCheckBox(_preferenceWidget);
    connect(cullCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setCullFace(int)));
    layout->addWidget(cullCheckBox, 3, 0);

    QCheckBox* lightCheckBox = new QCheckBox(_preferenceWidget);
    connect(lightCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLighting(int)));
    layout->addWidget(lightCheckBox, 3, 1);

    QPushButton* updateButton = new QPushButton(tr("Update faces"), _preferenceWidget);
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateFaces()));
    layout->addWidget(updateButton, 4, 0);


    tetrahedrons = new QCheckBox*[14];
    for (int i = 0; i < 14; i++)
    {
        tetrahedrons[i] = new QCheckBox(_preferenceWidget);
        layout->addWidget(tetrahedrons[i], 5+i, 0);
    }//for

    _preferenceWidget->setLayout(layout);
}//setupPreferenceWidget

