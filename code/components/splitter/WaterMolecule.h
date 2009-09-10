#pragma once

class WaterMolecule
{
    public:
        unsigned int OH2_coords[3];
        unsigned int H1_coords[3];
        unsigned int H2_coords[3];
        
        WaterMolecule(unsigned int* OH2, unsigned int* H1, unsigned int* H2);
};