#ifndef UTIL_H
#define UTIL_H

float* pack3f(float* v, float a, float b, float c);
float* sub2v(float* v1, float* v2, float* dst);
float* cross(float* v1, float* v2, float* dst);
float* multf(float* v, float f);
float len2(float* v);
float* normalize(float* v);

#endif
