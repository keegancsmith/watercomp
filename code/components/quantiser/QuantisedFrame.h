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
                       unsigned int z_subs);

        QuantisedFrame(unsigned int size,
                       unsigned int x_subs,
                       unsigned int y_subs,
                       unsigned int z_subs);
                       
        Frame toFrame() const;
        int natoms() const;

        std::vector<unsigned int> quantised_frame;

        float min_coord[3];
        float max_coord[3];

        unsigned int m_xquant, m_yquant, m_zquant;
};
