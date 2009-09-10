#include "WaterMolecule.h"


WaterMolecule::WaterMolecule(unsigned int* OH2, unsigned int* H1, unsigned int* H2)
{
    for(int d = 0; d < 3; ++d)
    {
        OH2_coords[d] = OH2[d];
        H1_coords[d] = H1[d];
        H2_coords[d] = H2[d];
    }
}