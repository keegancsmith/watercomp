#pragma once

#include "../pdbio/Frame.h"

#include <vector>

/// Might put this in one namespace

// Quantised Frame
class QuantisedFrame
{
    public:
        QuantisedFrame(const Frame & frame,
                       unsigned int x_subs,
                       unsigned int y_subs,
                       unsigned int z_subs,
                       float * min_coord = 0,
                       float * max_coord = 0);

        QuantisedFrame(unsigned int size,
                       unsigned int x_subs,
                       unsigned int y_subs,
                       unsigned int z_subs);

        Frame toFrame() const;
        int natoms() const;

        const unsigned int & at(size_t atom_idx, size_t d) const;
        unsigned int & at(size_t atom_idx, size_t d);

        std::vector<unsigned int> quantised_frame;

        float min_coord[3];
        float max_coord[3];

        unsigned int m_xquant, m_yquant, m_zquant;
};
