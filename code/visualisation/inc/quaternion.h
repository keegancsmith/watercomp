#ifndef QUATERNION
#define QUATERNION

class Quaternion
{
    public:
        double w, x, y, z;
        double matrix[16];

        Quaternion();
        Quaternion(const Quaternion& other);
        ~Quaternion();

        void local_rotation(double angle, double ax, double ay, double az);
        void rotate(double angle, double ax, double ay, double az);

        Quaternion operator*(const Quaternion& other);
        void mult(const Quaternion& other);

        void reset();
        void normalize();
        void update_matrix();
};//Quaternion

#endif
