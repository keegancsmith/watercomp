#pragma once

#include "../pdbio/AtomInformation.h"
#include "../pdbio/Frame.h"

#include <vector>

/// Might put this in one namespace

// Quantised Frame
class QFrame
{
    public:
        QFrame(const Frame & frame,
               unsigned int x_subs, unsigned int y_subs, unsigned int z_subs);

        std::vector<unsigned int> quantised_frame;
        std::vector<AtomInformation>* atom_information;        
        
        float min_coord[3];
        float max_coord[3];
};
