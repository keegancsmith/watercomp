#ifndef UTIL_H
#define UTIL_H

#include <GL/glu.h>

#define DEGTORAD 0.01745329251994329845f
#define RADTODEG 57.295779513082323f
#define HALF_ROOT_2 0.70710678118654757f
#define EPSILON 1E-6

#define RAD(x) ((x) * DEGTORAD)
#define DEG(x) ((x) * RADTODEG)


float* pack3f(float* v, float a, float b, float c);
float* sub2v(float* v1, float* v2, float* dst);
float* cross(float* v1, float* v2, float* dst);
float* multf(float* v, float f);
float len2(float* v);
float* normalize(float* v);
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric);

#endif
