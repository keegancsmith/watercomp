#include "Util.h"

#include <cmath>
#include <GL/gl.h>

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


void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric)
{
    float vx = x2-x1;
    float vy = y2-y1;
    float vz = z2-z1;

    //handle the degenerate case of z1 == z2 with an approximation
    if(vz == 0)
        vz = 1;
        // vz = .00000001;

    float v = sqrt( vx*vx + vy*vy + vz*vz );
    // float ax = 57.2957795*acos( vz/v );
    float ax = DEG(acos( vz/v ));
    if ( vz < 0.0 )
        ax = -ax;
    float rx = -vy*vz;
    float ry = vx*vz;
    glPushMatrix();

    //draw the cylinder body
    glTranslatef( x1,y1,z1 );
    glRotatef(ax, rx, ry, 0.0);
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluCylinder(quadric, radius, radius, v, subdivisions, 1);

    //draw the first cap
    // gluQuadricOrientation(quadric,GLU_INSIDE);
    // gluDisk( quadric, 0.0, radius, subdivisions, 1);
    gluSphere(quadric, radius, subdivisions, subdivisions);
    glTranslatef( 0,0,v );

    //draw the second cap
    // gluQuadricOrientation(quadric,GLU_OUTSIDE);
    // gluDisk( quadric, 0.0, radius, subdivisions, 1);
    gluSphere(quadric, radius, subdivisions, subdivisions);
    glPopMatrix();
}

