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

        // atom_data is the buffer used to store the frame.
        // Must be of size 3 * natoms()
        Frame toFrame(float * atom_data) const;
        int natoms() const;

        std::vector<unsigned int> quantised_frame;
        std::vector<AtomInformation>* atom_information;

        float min_coord[3];
        float max_coord[3];

        unsigned int m_xquant, m_yquant, m_zquant;
};
