#pragma once

#include "quantiser/QuantisedFrame.h"

#include <vector>

/// Might put this in one namespace
// Sorted Quantised Frame
class SortedQuantisedFrame
{
    public:
        SortedQuantisedFrame(const QuantisedFrame& frame);
               
        SortedQuantisedFrame() {};

        // atom_data is the buffer used to store the frame.
        // Must be of size 3 * natoms()
        QuantisedFrame toQFrame() const;
        int natoms() const;

        std::vector< std::pair<unsigned long long, unsigned int> > sorted_frame;

        float min_coord[3];
        float max_coord[3];
        
        unsigned int m_xquant, m_yquant, m_zquant;
};
