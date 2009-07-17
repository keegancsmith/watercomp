#include "Atom.h"
#define MAX(a, b) ((a) > (b) ? a : b)

using namespace std;

Atom Atom::quantise_atom(Atom& a, float quantisation)
{
    return Atom(int(a.x/quantisation), int(a.y/quantisation), int(a.z/quantisation));
}

Atom Atom::create_from_index(unsigned int index, int xbits, int ybits, int zbits, float min_x, float min_y, float min_z, float max_x, float max_y, float max_z)
{
    // Calculates information regarding step size of each coordinates octree indexs
    float ran_x = max_x - min_x;
    float ran_y = max_y - min_y;
    float ran_z = max_z - min_z;
    
    int max_x_quanta = 1<<xbits;
    int max_y_quanta = 1<<ybits;
    int max_z_quanta = 1<<zbits;
    
    float step_x = ran_x/max_x_quanta;
    float step_y = ran_y/max_y_quanta;
    float step_z = ran_z/max_z_quanta;
    
    int x_bits = 0;
    int y_bits = 0;
    int z_bits = 0;
    
    // Find coordinats from index
    int place = 0;
    unsigned int ix, iy, iz;
    ix = iy = iz = 0;
    
    while(x_bits != xbits || y_bits != ybits || z_bits != zbits)
    {
        if(x_bits != xbits)
        {
            int b = (index&(1 << place))>>place;
            ++place;
            ix = ix | (b << x_bits++);
        }
        
        if(y_bits != ybits)
        {
            int b = (index&(1 << place))>>place;
            ++place;
            iy = iy | (b << y_bits++);
        }
        
        if(z_bits != zbits)
        {
            int b = (index&(1 << place))>>place;
            ++place;
            iz = iz | (b << z_bits++);
        }
    }
    
    // Scale and shift them to be in approximately the correct positions
    return Atom(min_x + ix*step_x, min_y + iy*step_y, min_z + iz*step_z);
}
        
unsigned int Atom::get_index(int xbits, int ybits, int zbits)
{
    // Calculate the index of this corrected atom - MAY ditch this function
    int lim = MAX(MAX(xbits, ybits), zbits);
    int ix = x;
    int iy = y;
    int iz = z;
    unsigned int result = 0;
    
    int place = 0;

    for(int j = 0; j < lim; ++j)
    {
        int x_bit = (ix&(1<<j))>>j;
        int y_bit = (iy&(1<<j))>>j;
        int z_bit = (iz&(1<<j))>>j;
        
        if(j < xbits)
            result |= (x_bit<<place++);
        if(j < ybits)
            result |= (y_bit<<place++);
        if(j < zbits)
            result |= (z_bit<<place++);
    }
    
    return result;
}