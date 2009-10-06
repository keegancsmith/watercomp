#include "Quaternion.h"

#include <cmath>

Quaternion::Quaternion()
{
    reset();
}//default constructor

Quaternion::Quaternion(const Quaternion& other)
{
    w = other.w;
    x = other.x;
    y = other.y;
    z = other.z;
}//copy constructor

Quaternion::~Quaternion()
{
}//destructor

void Quaternion::local_rotation(double angle, double ax, double ay, double az)
{
    double a = angle * 0.5;
    w = cos(a);

    a = sin(a);
    x = ax * a;
    y = ay * a;
    z = az * a;

    normalize();
}//local_rotation

void Quaternion::rotate(double angle, double ax, double ay, double az)
{
    Quaternion q;
    q.local_rotation(angle, ax, ay, az);
    mult(q);
}//rotate

Quaternion Quaternion::operator*(const Quaternion& other)
{
    Quaternion q;
    q.w = w*other.w - x*other.x - y*other.y - z*other.z;
    q.x = w*other.x + x*other.w + y*other.z - z*other.y;
    q.y = w*other.y - x*other.z + y*other.w + z*other.x;
    q.z = w*other.z + x*other.y - y*other.x + z*other.w;
    return q;
}//operator*

void Quaternion::mult(const Quaternion& other)
{
    double tw = w*other.w - x*other.x - y*other.y - z*other.z;
    double tx = w*other.x + x*other.w + y*other.z - z*other.y;
    double ty = w*other.y - x*other.z + y*other.w + z*other.x;
    double tz = w*other.z + x*other.y - y*other.x + z*other.w;

    w = tw;
    x = tx;
    y = ty;
    z = tz;
}//operator*

void Quaternion::reset()
{
    w = 1;
    x = 0;
    y = 0;
    z = 0;
}//reset

void Quaternion::normalize()
{
    double m = w*w + x*x + y*y + z*z;
    if ((m > 1.2) || (m < 0.9))
    {
        m = sqrt(m);
        w /= m;
        x /= m;
        y /= m;
        z /= m;
    }//if
}//normalize

void Quaternion::update_matrix()
{
    double x2 = x*x;
    double y2 = y*y;
    double z2 = z*z;

    matrix[0] = 1 - 2 * (y2 + z2);
    matrix[1] = 2 * (x*y - w*z);
    matrix[2] = 2 * (x*z + w*y);
    matrix[3] = 0;

    matrix[4] = 2 * (x*y + w*z);
    matrix[5] = 1 - 2 * (x2 + z2);
    matrix[6] = 2 * (y*z - w*x);
    matrix[7] = 0;

    matrix[8] = 2 * (x*z - w*y);
    matrix[9] = 2 * (y*z + w*x);
    matrix[10] = 1 - 2 * (x2 + y2);
    matrix[11] = 0;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}//update_matrix

