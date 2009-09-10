#pragma once
#include <vector>
#include "../pdb_reader/AtomInformation.h"

/// Might put this in one namespace

class QFrame
{
    public:
        std::vector<unsigned int> quantised_frame;
        
        float min_coord[3];
        float max_coord[3];

        std::vector<AtomInformation>* atom_information;        

        QFrame(float* atom_data, size_t atoms, std::vector<AtomInformation>* n_atom_information, unsigned int x_subs, unsigned int y_subs, unsigned int z_subs);
};
