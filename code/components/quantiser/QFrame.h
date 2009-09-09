#pragma once
#include <vector>

class QFrame
{
    public:
        std::vector<unsigned int> quantised_frame;
        
        float min_coord[3];
        float max_coord[3];
        
        QFrame(float* atom_data, size_t atoms, unsigned int x_subs, unsigned int y_subs, unsigned int z_subs);
};