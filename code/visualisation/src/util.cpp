#include "util.h"

#include <cmath>

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

