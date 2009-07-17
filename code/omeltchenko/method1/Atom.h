#pragma once

class Atom
{
    public:
        /* Coordinates of the atom (possibly quantised) */
        float x, y, z;
        
        /* Typical Constructor */
        Atom(float tx = 0.0, float ty = 0.0, float tz = 0.0) : x(tx), y(ty), z(tz) {};
        
        /* Not used: Divides each coordinate by the quantisation factor and truncates them */
        static Atom quantise_atom(Atom& a, float quantisation);
        
        /* Creates an atom from an octree index, with some extra information */
        static Atom create_from_index(unsigned int index, int xbits, int ybits, int zbits, float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);
        
        /* Calculates the octree index from atom using the number of bits for each component */
        unsigned int get_index(int xbits, int ybits, int zbits);
};