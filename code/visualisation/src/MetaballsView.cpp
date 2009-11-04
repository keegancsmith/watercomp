#include "MetaballsView.h"

#include <GL/gl.h>
#include <sys/time.h>
#include <cassert>
#include <cmath>

#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

#include <pdbio/DCDReader.h>
#include <quantiser/QuantisedFrame.h>

#define ALPHA_MAX_SLIDER 100
#define ALPHA_MAX_VAL 1.0

#define MAX_QUANT 8

#include "MarchingTables.cpp"
#include "Renderer.h"
#include "Util.h"


int MetaballsView::surface_extraction = 0;


bool Point3f::operator<(const Point3f& p) const
{
    if (fabs(x - p.x) < 1e-6)
    {
        if (fabs(y - p.y) < 1e-6)
        {
            if (fabs(z - p.z) < 1e-6)
            {
                return false;
            }//if
            return z < p.z;
        }//if
        return y < p.y;
    }//if
    return x < p.x;
}//operator<


#ifndef USE_GRID

bool operator==(const Point3f& p1, const Point3f& p2)
{
    if ((fabs(p1.x - p2.x) > 1e-6) ||
        (fabs(p1.y - p2.y) > 1e-6) ||
        (fabs(p1.z - p2.z) > 1e-6))
        return false;
    return true;
}//operator==

uint qHash(const Point3f& p)
{
    // some arb 3 float function
// #define HF(x) (int)((x)*1000)
    // return HF(p.x) ^ HF(p.y) ^ HF(p.z)
// #undef
    return p.x*p.x + p.y*p.y + p.z*p.z;
}//qHash

#endif

double wtime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + 1.0e-6*tv.tv_usec;
}//wtime



void sampleSphere(gdouble** f, GtsCartesianGrid g, guint k, gpointer data)
{
    double x, y, z = g.z;
    int i, j;
    float dd, dx, dy, dz = z - 80;
    float r2 = 300.0 * 255;
    for (i = 0, x = g.x; i < g.nx; i++, x+=g.dx)
    {
        dx = x - 100;
        for (j = 0, y = g.y; j < g.ny; j++, y+=g.dy)
        {
            dy = y - 80;
            dd = dx*dx + dy*dy + dz*dz;
            f[i][j] = (dd <= 0.001) ? 255 : r2 / dd;
        }//for
    }//for
}//sampleVolume

int draw_face(gpointer item, gpointer data)
{
    QVector<Triangle>* surface = (QVector<Triangle>*)data;
    GtsFace* face = (GtsFace*)item;
    GtsPoint *v1, *v2, *v3;
    GtsPoint *p1, *p2;
    v1 = &(face->triangle.e1->segment.v1->p);
    v2 = &(face->triangle.e1->segment.v2->p);

    p1 = &(face->triangle.e2->segment.v1->p);
    p2 = &(face->triangle.e2->segment.v2->p);
    // stupid winding
    if ((p1 == v1) || (p2 == v1))
    {
        v3 = (p1 == v1) ? p2 : p1;
    }//if
    else if ((p1 == v2) || (p2 == v2))
    {
        v3 = v1;
        v1 = v2;
        v2 = v3;
        v3 = (p1 == v1) ? p2 : p1;
    }//else
    else
    {
        printf("um: (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f)\n",
                v1->x, v1->y, v1->z,
                v2->x, v2->y, v2->z,
                p1->x, p1->y, p1->z,
                p2->x, p2->y, p2->z
                );
        return 0;
    }//else
    if ((v1 == v2) || (v1 == v3) || (v2 == v3))
    {
        printf("whoa: (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f)\n",
                v1->x, v1->y, v1->z,
                v2->x, v2->y, v2->z,
                v3->x, v3->y, v3->z,
                p1->x, p1->y, p1->z,
                p2->x, p2->y, p2->z
                );
        assert(false);
        // return 0;
    }//else

    // stupid stupid winding
    if (MetaballsView::surface_extraction)
    {
        p1 = v1;
        v1 = v2;
        v2 = p1;
    }//if

    float e1[] = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};
    float e2[] = {v1->x - v3->x, v1->y - v3->y, v1->z - v3->z};
    float e3[3];
    normalize(cross(e1, e2, e3));

    Triangle t;
    pack3f(t.p[0], v1->x, v1->y, v1->z);
    pack3f(t.p[1], v2->x, v2->y, v2->z);
    pack3f(t.p[2], v3->x, v3->y, v3->z);
    pack3f(t.n[0], e3[0], e3[1], e3[2]);
    pack3f(t.n[1], e3[0], e3[1], e3[2]);
    pack3f(t.n[2], e3[0], e3[1], e3[2]);
    surface->push_back(t);

    return 0;
}//draw_face


void sample_volume_data(gdouble** f, GtsCartesianGrid g, guint k, gpointer data)
{
#ifdef USE_GRID
    unsigned char*** volume = (unsigned char***)data;
    int x, y, z = g.z;
    int i, j;
    for (i = 0, x = g.x; i < g.nx; i++, x+=g.dx)
        for (j = 0, y = g.y; j < g.ny; j++, y+=g.dy)
            f[i][j] = volume[z][y][x];
#else
    QHash<Point3f, int>* volume = (QHash<Point3f, int>*)data;
    Point3f p3f;
    p3f.z = g.z;
    int i, j;
    for (i = 0, p3f.x = g.x; i < g.nx; i++, p3f.x+=g.dx)
        for (j = 0, p3f.y = g.y; j < g.ny; j++, p3f.y+=g.dy)
            f[i][j] = (*volume)[p3f];
#endif
}//sample_volume_data


int process_vertex(MetaballsView* view, float* vertex, float* normal)
// int process_vertex(MetaballsView* view, float x, float y, float z, float* n)
// int process_vertex(MetaballsView* view, GtsPoint* p, float* n)
{
    int v;
    int i;
    // Point3f p; p.x = x; p.y = y; p.z = z;
    Point3f p; p.x = vertex[0]; p.y = vertex[1]; p.z = vertex[2];
    if (view->vertex_map.find(p) == view->vertex_map.end())
    {
        v = view->vertex_num++;
        view->vertex_map[p] = v;
        i = v*3;
        view->vertices[i]   = p.x;
        view->vertices[i+1] = p.y;
        view->vertices[i+2] = p.z;
        i = v*4;
        view->avg_normals[i]   = normal[0];
        view->avg_normals[i+1] = normal[1];
        view->avg_normals[i+2] = normal[2];
        view->avg_normals[i+3] = 1;
    }//if
    else
    {
        v = view->vertex_map[p];
        i = v*4;
        view->avg_normals[i]   += normal[1];
        view->avg_normals[i+1] += normal[2];
        view->avg_normals[i+2] += normal[3];
        view->avg_normals[i+3] += 1;
    }//else
    return v;
}//process_vertex

int process_surface(gpointer item, gpointer data)
{
    MetaballsView* view = (MetaballsView*)data;

    GtsFace* face = (GtsFace*)item;
    GtsPoint *v1, *v2, *v3;
    GtsPoint *p1, *p2;
    // printf("0x%x\n", face->triangle.e1);
    v1 = &(face->triangle.e1->segment.v1->p);
    v2 = &(face->triangle.e1->segment.v2->p);

    p1 = &(face->triangle.e2->segment.v1->p);
    p2 = &(face->triangle.e2->segment.v2->p);
    // stupid winding
    if ((p1 == v1) || (p2 == v1))
    {
        v3 = (p1 == v1) ? p2 : p1;
    }//if
    else if ((p1 == v2) || (p2 == v2))
    {
        v3 = v1;
        v1 = v2;
        v2 = v3;
        v3 = (p1 == v1) ? p2 : p1;
    }//else
    else
    {
        printf("um\n");
        // printf("um: (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f)\n",
                // v1->x, v1->y, v1->z,
                // v2->x, v2->y, v2->z,
                // p1->x, p1->y, p1->z,
                // p2->x, p2->y, p2->z
                // );
        return 0;
    }//else
    if ((v1 == v2) || (v1 == v3) || (v2 == v3))
    {
        printf("whoa: (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f)\n",
                v1->x, v1->y, v1->z,
                v2->x, v2->y, v2->z,
                v3->x, v3->y, v3->z,
                p1->x, p1->y, p1->z,
                p2->x, p2->y, p2->z
                );
        assert(false);
        // return 0;
    }//else

    float e1[] = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};
    float e2[] = {v1->x - v3->x, v1->y - v3->y, v1->z - v3->z};
    float e3[3];
    normalize(cross(e1, e2, e3));

    int t = view->triangle_num++;
    t *= 3;
    // view->indices[t]   = process_vertex(view, v1, e3);
    // view->indices[t+1] = process_vertex(view, v2, e3);
    // view->indices[t+2] = process_vertex(view, v3, e3);
    e1[0] = v1->x; e1[1] = v1->y; e1[2] = v1->z;
    view->indices[t]   = process_vertex(view, e1, e3);

    e1[0] = v2->x; e1[1] = v2->y; e1[2] = v2->z;
    view->indices[t+1] = process_vertex(view, e1, e3);

    e1[0] = v3->x; e1[1] = v3->y; e1[2] = v3->z;
    view->indices[t+2] = process_vertex(view, e1, e3);

    // Triangle t;
    // pack3f(t.p[0], v1->x, v1->y, v1->z);
    // pack3f(t.p[1], v2->x, v2->y, v2->z);
    // pack3f(t.p[2], v3->x, v3->y, v3->z);
    // pack3f(t.n[0], e3[0], e3[1], e3[2]);
    // pack3f(t.n[1], e3[0], e3[1], e3[2]);
    // pack3f(t.n[2], e3[0], e3[1], e3[2]);
    // surface->push_back(t);

    // glNormal3fv(e3);
    // glVertex3d(v1->x, v1->y, v1->z);
    // glVertex3d(v2->x, v2->y, v2->z);
    // glVertex3d(v3->x, v3->y, v3->z);

    return 0;
}//process_surface


MetaballsView::MetaballsView()
{
    settings = new QSettings;
    maxStepSize = 80;
    _stepSize = settings->value("MetaballsView/stepSize", 5).toInt();

    viewName = "Metaballs view";
    _metaballsColor[0] = settings->value("MetaballsView/colorR", 0.5).toDouble();
    _metaballsColor[1] = settings->value("MetaballsView/colorG", 0.5).toDouble();
    _metaballsColor[2] = settings->value("MetaballsView/colorB", 0.5).toDouble();
    _metaballsColor[3] = settings->value("MetaballsView/colorA", 1.0).toDouble();
    quantised = 0;

    lighting = settings->value("MetaballsView/lighting", false).toBool();
    cullFace = settings->value("MetaballsView/cullFace", false).toBool();
    haveDataFile = false;
    useDataFile = settings->value("MetaballsView/dataFile", true).toBool();
    isoValue = settings->value("MetaballsView/isoValue", 100).toInt();
    decimateSurface = settings->value("MetaballsView/decimateSurface", false).toBool();
    surfaceExtraction = settings->value("MetaballsView/surfaceExtraction", 0).toInt();
    MetaballsView::surface_extraction = surfaceExtraction;

    g_surface = 0;

    g_grid.x = 0;
    g_grid.y = 0;
    g_grid.z = 0;

    vertices = 0;
    normals = 0;
    avg_normals = 0;
    indices = 0;
    triangle_num = 0;
    vertex_num = 0;

    doSplitWaters = true;

    meta_file = 0;
    meta_data = 0;
#ifdef USE_GRID
    size = 1 << MAX_QUANT;

    int x, y, z;
    volumedata = new unsigned char**[size];
    for (z = 0; z < size; z++)
    {
        volumedata[z] = new unsigned char*[size];
        for (y = 0; y < size; y++)
        {
            volumedata[z][y] = new unsigned char[size];
            for (x = 0; x < size; x++)
                volumedata[z][y][x] = 0;
        }//for
    }//for
#endif
}//constructor

MetaballsView::~MetaballsView()
{
    if (meta_file != 0)
    {
        meta_file->close();
        delete meta_file;
    }//if
    if (meta_data != 0)
        delete meta_data;
    settings->sync();
    delete settings;
#ifdef USE_GRID
    int x, y, z;
    for (z = 0; z < size; z++)
    {
        for (y = 0; y < size; y++)
            delete [] volumedata[z][y];
        delete [] volumedata[z];
    }//for
    delete [] volumedata;
#endif
}//destructor


int MetaballsView::stepSize()
{
    return _stepSize;
}//stepSize


void MetaballsView::updatePreferences()
{
    metaballsAlphaSlider->setValue((int)(_metaballsColor[3] * ALPHA_MAX_SLIDER / ALPHA_MAX_VAL));
    stepSizeSpinBox->setValue(_stepSize);
    lightCheckBox->setCheckState(lighting ? Qt::Checked : Qt::Unchecked);
    cullCheckBox->setCheckState(cullFace ? Qt::Checked : Qt::Unchecked);
    dataFileCheckBox->setCheckState(useDataFile ? Qt::Checked : Qt::Unchecked);
    dataFileCheckBox->setEnabled(haveDataFile);
    isoValueSpinBox->setValue(isoValue);
    decimateSurfaceCheckBox->setCheckState(decimateSurface ? Qt::Checked : Qt::Unchecked);
    surfaceExtractionComboBox->setCurrentIndex(surfaceExtraction);
}//updatePreferences

void MetaballsView::setupPreferenceWidget(QWidget* preferenceWidget)
{
    QGridLayout* layout = new QGridLayout(preferenceWidget);

    QPushButton* metaballsColorButton = new QPushButton(tr("Select metaballs colour"), preferenceWidget);
    connect(metaballsColorButton, SIGNAL(clicked()), this, SLOT(pickMetaballsColor()));
    layout->addWidget(metaballsColorButton, 0, 0, 1, 2);

    QLabel* metaballsAlphaLabel = new QLabel(tr("Metaballs alpha"), preferenceWidget);
    layout->addWidget(metaballsAlphaLabel, 1, 0);

    metaballsAlphaSlider = new QSlider(preferenceWidget);
    metaballsAlphaSlider->setOrientation(Qt::Horizontal);
    metaballsAlphaSlider->setRange(0, ALPHA_MAX_SLIDER);
    connect(metaballsAlphaSlider, SIGNAL(valueChanged(int)), this, SLOT(setMetaballsAlpha(int)));
    layout->addWidget(metaballsAlphaSlider, 1, 1);

    QLabel* stepSizeLabel = new QLabel(tr("Step size"), preferenceWidget);
    layout->addWidget(stepSizeLabel, 2, 0);

    stepSizeSpinBox = new QSpinBox(preferenceWidget);
    stepSizeSpinBox->setRange(2, maxStepSize);
    connect(stepSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setStepSize(int)));
    layout->addWidget(stepSizeSpinBox, 2, 1);

    QLabel* cullLabel = new QLabel(tr("Cull faces"), preferenceWidget);
    layout->addWidget(cullLabel, 3, 0);

    cullCheckBox = new QCheckBox(preferenceWidget);
    connect(cullCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setCullFace(int)));
    layout->addWidget(cullCheckBox, 3, 1);

    QLabel* lightLabel = new QLabel(tr("Enable lighting"), preferenceWidget);
    layout->addWidget(lightLabel, 4, 0);

    lightCheckBox = new QCheckBox(preferenceWidget);
    connect(lightCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLighting(int)));
    layout->addWidget(lightCheckBox, 4, 1);

    QLabel* dataFileLabel = new QLabel(tr("Use data file"), preferenceWidget);
    layout->addWidget(dataFileLabel, 5, 0);

    dataFileCheckBox = new QCheckBox(preferenceWidget);
    connect(dataFileCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setUseDataFile(int)));
    layout->addWidget(dataFileCheckBox, 5, 1);

    QLabel* isoValueLabel = new QLabel(tr("Iso value"), preferenceWidget);
    layout->addWidget(isoValueLabel, 6, 0);

    isoValueSpinBox = new QSpinBox(preferenceWidget);
    isoValueSpinBox->setRange(0, 255);
    connect(isoValueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setIsoValue(int)));
    layout->addWidget(isoValueSpinBox, 6, 1);

    QLabel* decimateSurfaceLabel = new QLabel(tr("Decimate surface"), preferenceWidget);
    layout->addWidget(decimateSurfaceLabel, 7, 0);

    decimateSurfaceCheckBox = new QCheckBox(preferenceWidget);
    connect(decimateSurfaceCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setDecimateSurface(int)));
    layout->addWidget(decimateSurfaceCheckBox, 7, 1);

    QLabel* surfaceExtractionLabel = new QLabel(tr("Surface extraction"), preferenceWidget);
    layout->addWidget(surfaceExtractionLabel, 8, 0);

    surfaceExtractionComboBox = new QComboBox(preferenceWidget);
    surfaceExtractionComboBox->addItem(tr("Marching cubes"));
    surfaceExtractionComboBox->addItem(tr("Marching tetrahedrons"));
    surfaceExtractionComboBox->addItem(tr("Marching tetrahedrons bcl"));
    connect(surfaceExtractionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSurfaceExtraction(int)));
    layout->addWidget(surfaceExtractionComboBox, 8, 1);

    QPushButton* updateButton = new QPushButton(tr("Update faces"), preferenceWidget);
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateFaces()));
    layout->addWidget(updateButton, 9, 0);

    preferenceWidget->setLayout(layout);
}//setupPreferenceWidget


void MetaballsView::initGL()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float l0_amb[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float l0_dif[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float l0_spe[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);

    float l1_amb[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float l1_dif[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float l1_spe[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, l1_amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, l1_spe);

    float backAmbient[] = {0.00, 0.25, 0.00, 1.00};
    float backDiffuse[] = {0.00, 0.75, 0.00, 1.00};
    float frontAmbient[] = {0.00, 0.00, 0.25, 1.00};
    float frontDiffuse[] = {0.00, 0.15, 0.75, 1.00};
    // float specularWhite[] = {1.00, 1.00, 1.00, 1.00};

    glMaterialfv(GL_BACK,  GL_AMBIENT,   backAmbient);
    glMaterialfv(GL_BACK,  GL_DIFFUSE,   backDiffuse);
    // glMaterialfv(GL_BACK, GL_SPECULAR,  whiteSpecular);
    // glMaterialf(GL_BACK, GL_SHININESS, 15.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
    // glMaterialfv(GL_FRONT, GL_SPECULAR,  whiteSpecular);
    // glMaterialf(GL_FRONT, GL_SHININESS, 15.0);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    if (lighting) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);

    if (cullFace) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}//initGL

void MetaballsView::tick(int framenum, Frame* unquantised, QuantisedFrame* quantised, Frame* dequantised)
{
    BaseView::tick(framenum, unquantised, quantised, dequantised);
    if (dequantised == 0) return;
    if (useDataFile) return;

    double start_time = wtime();
    g_grid.nx = 1 << MAX_QUANT;
    g_grid.ny = 1 << MAX_QUANT;
    g_grid.nz = 1 << MAX_QUANT;

    g_grid.dx = _stepSize;
    g_grid.dy = _stepSize;
    g_grid.dz = _stepSize;

    g_grid.nx /= g_grid.dx;
    g_grid.ny /= g_grid.dy;
    g_grid.nz /= g_grid.dz;

    int max_coord[] = {1 << MAX_QUANT, 1 << MAX_QUANT, 1 << MAX_QUANT};
    int z, y, x;
#ifdef USE_GRID
    for (z = 0; z < max_coord[2]; ++z)
        for (y = 0; y < max_coord[1]; ++y)
            for (x = 0; x < max_coord[0]; ++x)
                volumedata[z][y][x] = 0;
#else
    meta_volume.clear();
    Point3f p3f;
#endif

    int start_coord[3];
    int final_coord[3];
    float metaballs_ratio = 0.06;
    int metaballs_size[] = {max_coord[0] * metaballs_ratio,
                            max_coord[1] * metaballs_ratio,
                            max_coord[2] * metaballs_ratio};
    int contrib = 10;
    int maxval = 255;
    int min_dataval = maxval;
    int max_dataval = 0;
    int v, c;
    QuantisedFrame meta_positions(*dequantised, MAX_QUANT, MAX_QUANT, MAX_QUANT);
    for (int i = 0; i < waters.size(); ++i)
    {
        for (c = 0; c < 3; ++c)
        {
            v = meta_positions.quantised_frame[3*waters[i].OH2_index+c];
            start_coord[c] = v - metaballs_size[c];
            if (start_coord[c] < 0) start_coord[c] = 0;
            final_coord[c] = v + metaballs_size[c];
            if (final_coord[c] > max_coord[c]) final_coord[c] = max_coord[c];
        }//for

        for (z = start_coord[2]; z < final_coord[2]; ++z)
            for (y = start_coord[1]; y < final_coord[1]; ++y)
                for (x = start_coord[0]; x < final_coord[0]; ++x)
                {
#ifdef USE_GRID
                    v = volumedata[z][y][x] + contrib;
                    volumedata[z][y][x] = (v > maxval) ? maxval : v;
#else
                    p3f.x = x;
                    p3f.y = y;
                    p3f.z = z;
                    v = meta_volume[p3f] + contrib;
                    meta_volume[p3f] = (v > maxval) ? maxval : v;
#endif
                    if (v < min_dataval) min_dataval = v;
                    if (v > max_dataval) max_dataval = v;
                }//for
    }//for

    _surface.clear();
    if (g_surface) gts_object_destroy((GtsObject*)g_surface);
    // if (g_surface) delete g_surface;
    g_surface = gts_surface_new(gts_surface_class(),
                                gts_face_class(),
                                gts_edge_class(),
                                gts_vertex_class());

#ifdef USE_GRID
    switch (surfaceExtraction)
    {
        case 0:
            gts_isosurface_cartesian(g_surface, g_grid, sample_volume_data, (void*)volumedata, isoValue);
            break;
        case 1:
            gts_isosurface_tetra(g_surface, g_grid, sample_volume_data, (void*)volumedata, isoValue);
            break;
        case 2:
            gts_isosurface_tetra_bcl(g_surface, g_grid, sample_volume_data, (void*)volumedata, isoValue);
            break;
    }//switch
#else
    gts_isosurface_cartesian(g_surface, g_grid, sample_volume_data, (void*)&meta_volume, isoValue);
    gts_isosurface_tetra(g_surface, g_grid, sample_volume_data, (void*)&meta_volume, isoValue);
    gts_isosurface_tetra_bcl(g_surface, g_grid, sample_volume_data, (void*)&meta_volume, isoValue);
#endif

    if (decimateSurface)
    {
        GtsVolumeOptimizedParams params = {0.5, 0.5, 0.0};

        GtsKeyFunc cost_func = (GtsKeyFunc)gts_volume_optimized_cost;
        gpointer cost_data = &params;

        GtsCoarsenFunc coarsen_func = (GtsCoarsenFunc)gts_volume_optimized_vertex;
        gpointer coarsen_data = &params;

            // GtsStopFunc stop_func = (GtsStopFunc)gts_coarsen_stop_cost;
            // gdouble cmax = 0.001;
            // gpointer stop_data = &cmax;
        GtsStopFunc stop_func = (GtsStopFunc)gts_coarsen_stop_number;
        guint number = gts_surface_face_number(g_surface) * 0.75;
        gpointer stop_data = &number;

        gts_surface_coarsen(g_surface,
                cost_func, cost_data,
                coarsen_func, coarsen_data,
                stop_func, stop_data,
                M_PI/180);
    }//if

    // now get the surface
    gts_surface_foreach_face(g_surface, draw_face, (void*)&_surface);
    double end_time = wtime();
    printf("\r%d: %d (%d~%d) waters:%d time:%lf ",
            framenum,
            _surface.size(),
            min_dataval,
            max_dataval,
            waters.size(),
            end_time - start_time);
    fflush(stdout);
    return;

    /*

    return;

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

    bool w = true;
    for (int i = 0; i < _surface.size(); i++)
    {
        t = _surface.at(i);
        for (v = 0; v < 3; v++)
        {
            if ((t.p[v][0] < 5) && w)
            {
                printf("wtf?\n");
                w = false;
            }//if

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

    // */

}//tick

void MetaballsView::render()
{
    if (useDataFile)
    {
        if (meta_data == 0) return;
    }//if
    else if (dequantised == 0) return;

    float l0_pos[] = {-1.0f, 1.0f, 2.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

    float l1_pos[] = {0.0f, 0.0f, -1.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);

    if (parent) glTranslatef(parent->volume_middle[0], parent->volume_middle[1], parent->volume_middle[2]);
    glTranslatef(-128, -128, -350);

    glColor4fv(_metaballsColor);
    glBegin(GL_TRIANGLES);

    /*

    int v;
    for (int i = 0; i < triangle_num; i++)
    {
        v = i*3;
        // printf("normal: %f %f %f\n", normals[indices[v]], normals[indices[v+1]], normals[indices[v+2]]);
        // printf("vertice: %f %f %f\n", vertices[indices[v]], vertices[indices[v+1]], vertices[indices[v+2]]);
        glNormal3f(normals[indices[v]],
                normals[indices[v+1]],
                normals[indices[v+2]]);
        glVertex3f(vertices[indices[v]],
                vertices[indices[v+1]],
                vertices[indices[v+2]]);
        v += 1;
        // printf("normal: %f %f %f\n", normals[indices[v]], normals[indices[v+1]], normals[indices[v+2]]);
        // printf("vertice: %f %f %f\n", vertices[indices[v]], vertices[indices[v+1]], vertices[indices[v+2]]);
        glNormal3f(normals[indices[v]],
                normals[indices[v+1]],
                normals[indices[v+2]]);
        glVertex3f(vertices[indices[v]],
                vertices[indices[v+1]],
                vertices[indices[v+2]]);
        v += 1;
        // printf("normal: %f %f %f\n", normals[indices[v]], normals[indices[v+1]], normals[indices[v+2]]);
        // printf("vertice: %f %f %f\n", vertices[indices[v]], vertices[indices[v+1]], vertices[indices[v+2]]);
        glNormal3f(normals[indices[v]],
                normals[indices[v+1]],
                normals[indices[v+2]]);
        glVertex3f(vertices[indices[v]],
                vertices[indices[v+1]],
                vertices[indices[v+2]]);
    }//for

    // */

    if (useDataFile)
    {
        quint32 size;
        // jump around in the file to find the frame positions
        // and do this lazily
        while (lastframe < framenum)
        {
            (*meta_data) >> size;
            //3 vertices
            //3 coords
            //a value and a normal
            meta_file->seek(meta_file->pos() +
                    size * 3 * 3 * (sizeof(float) + sizeof(float)));
            meta_pos[++lastframe] = meta_file->pos();
        }//if

        int j, k, l;
        // if the loaded frame is not the current frame to show
        // then jump to the position and load the data
        if (loadedframe != framenum)
        {
            meta_file->seek(meta_pos[framenum]);
            loadedframe = framenum;
            (*meta_data) >> size;
            _surface.resize(size);
            for (j = 0; j < size; ++j)
            {
                Triangle t;
                for (k = 0; k < 3; ++k)
                    for (l = 0; l < 3; ++l)
                        (*meta_data) >> t.p[k][l];
                for (k = 0; k < 3; ++k)
                    for (l = 0; l < 3; ++l)
                        (*meta_data) >> t.n[k][l];
                _surface[j] = t;
            }//for
            // just read a frame of data, so check if should update
            // the meta_pos and last frame data
            if (lastframe == framenum)
                meta_pos[++lastframe] = meta_file->pos();
            printf("\r%d: %d ", framenum, _surface.size());
            fflush(stdout);
        }//if
    }//if

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



void MetaballsView::setMetaballsAlpha(int value)
{
    if (value > ALPHA_MAX_SLIDER) value = ALPHA_MAX_SLIDER;
    if (value < 0) value = 0;
    _metaballsColor[3] = (float)value * ALPHA_MAX_VAL / ALPHA_MAX_SLIDER;

    settings->setValue("MetaballsView/colorA", _metaballsColor[3]);
}//setMetaballsAlpha

void MetaballsView::setStepSize(int value)
{
    _stepSize = value;

    g_grid.dx = _stepSize;
    g_grid.dy = _stepSize;
    g_grid.dz = _stepSize;

    settings->setValue("MetaballsView/stepSize", _stepSize);
}//setStepSize

void MetaballsView::pickMetaballsColor()
{
    pickColor(_metaballsColor);

    settings->setValue("MetaballsView/colorR", _metaballsColor[0]);
    settings->setValue("MetaballsView/colorG", _metaballsColor[1]);
    settings->setValue("MetaballsView/colorB", _metaballsColor[2]);
}//pickMetaballsColor

void MetaballsView::setCullFace(int state)
{
    cullFace = state != 0;
    settings->setValue("MetaballsView/cullFace", cullFace);

    if (current)
    {
        if (cullFace)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }//if
}//setCullFace

void MetaballsView::setLighting(int state)
{
    lighting = state != 0;
    settings->setValue("MetaballsView/lighting", lighting);

    if (current)
    {
        if (lighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
    }//if
}//setLighting

void MetaballsView::setUseDataFile(int state)
{
    useDataFile = state != 0;
    settings->setValue("MetaballsView/dataFile", useDataFile);
}//setUseDataFile

void MetaballsView::setIsoValue(int value)
{
    isoValue = value;
    settings->setValue("MetaballsView/isoValue", isoValue);
}//setUseDataFile

void MetaballsView::setDecimateSurface(int state)
{
    decimateSurface = state != 0;
    settings->setValue("MetaballsView/decimateSurface", decimateSurface);
}//setDecimateSurface

void MetaballsView::setSurfaceExtraction(int value)
{
    surfaceExtraction = value;
    settings->setValue("MetaballsView/surfaceExtraction", surfaceExtraction);
    MetaballsView::surface_extraction = surfaceExtraction;
}//setSurfaceExtraction

void MetaballsView::updateFaces()
{
    tick(this->framenum, this->unquantised, this->quantised, this->dequantised);
}//updateFaces


float MetaballsView::sampleVolume(float x, float y, float z)
{
#ifdef USE_GRID
    int ix = (int)x;
    int iy = (int)y;
    int iz = (int)z;
    return volumedata[iz][iy][ix];
#else
    return 0;
#endif
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





bool MetaballsView::processVolume(QString filename, DCDReader* reader, int quant_level)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << (quint32)reader->nframes();

    float* atoms = new float[3 * reader->natoms()];
    Frame* unquant = new Frame(atoms, reader->natoms());
    QuantisedFrame* quant = new QuantisedFrame(1, 1, 1, 1);;
    Frame* dequant = new Frame(quant->toFrame());

    useDataFile = false;
    int v;
    int j, k, l;
    Triangle t;
    int nframes = reader->nframes();
    printf("\nFrames: %d\n", nframes);
    for (int i = 0; i < nframes; ++i)
    {
        reader->set_frame(i);
        reader->next_frame(*unquant);

        delete quant; quant = new QuantisedFrame(*unquant, quant_level, quant_level, quant_level);
        delete dequant; dequant = new Frame(quant->toFrame());
        tick(i, unquant, quant, dequant);

        out << (quint32)_surface.size();
        for (j = 0; j < _surface.size(); ++j)
        {
            t = _surface.at(j);
            for (k = 0; k < 3; ++k)
                for (l = 0; l < 3; ++l)
                    out << t.p[k][l];
            for (k = 0; k < 3; ++k)
                for (l = 0; l < 3; ++l)
                    out << t.n[k][l];
        }//for
        printf("%lld  ", file.pos());
    }//for

    delete unquant;
    delete quant;
    delete dequant;

    file.close();
    return true;
}//processVolume


bool MetaballsView::loadFile(QString filename)
{
    haveDataFile = false;
    if (meta_file != 0)
    {
        meta_file->close();
        delete meta_file;
    }//if
    meta_file = new QFile(filename);
    if (!meta_file->open(QIODevice::ReadOnly))
    {
        meta_file->close();
        delete meta_file;
        meta_file = 0;
        useDataFile = false;
        return false;
    }//if
    haveDataFile = true;

    if (meta_data != 0)
        delete meta_data;
    meta_data = new QDataStream(meta_file);

    int total_size, size;
    (*meta_data) >> total_size;

    meta_pos.resize(total_size+1);
    meta_pos[0] = meta_file->pos();
    lastframe = 0;
    loadedframe = -1;
    return true;
}//loadFile

