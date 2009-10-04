#include "SortedQuantisedFrame.h"
#include <algorithm>

using namespace std;

SortedQuantisedFrame::SortedQuantisedFrame(const QuantisedFrame& frame)
{
    m_xquant = frame.m_xquant;
    m_yquant = frame.m_yquant;
    m_zquant = frame.m_zquant;
    
    for(int d = 0; d < 3; ++d)
    {
        min_coord[d] = frame.min_coord[d];
        max_coord[d] = frame.max_coord[d];
    }
    
    unsigned int loop_limit = max(max(m_xquant, m_yquant), m_zquant);
    
    for(size_t i = 0; i < frame.natoms(); ++i)
    {
        unsigned int result = 0;
        unsigned int bit = 0;
        
        unsigned int x = frame.quantised_frame[3*i];
        unsigned int y = frame.quantised_frame[3*i + 1];
        unsigned int z = frame.quantised_frame[3*i + 2];
        
        for(int j = 0; j < loop_limit; ++j)
        {
            unsigned int x_bit = (x&(1<<j))>>j;
            unsigned int y_bit = (y&(1<<j))>>j;
            unsigned int z_bit = (z&(1<<j))>>j;
            
            if(j < m_xquant)
                result |= (x_bit<<bit++);
            if(j < m_yquant)
                result |= (y_bit<<bit++);
            if(j < m_zquant)
                result |= (z_bit<<bit++);
        }
        
        sorted_frame.push_back(std::pair<unsigned int, unsigned int>(result, i));
    }
    
    std::sort(sorted_frame.begin(), sorted_frame.end());
}

// atom_data is the buffer used to store the frame.
// Must be of size 3 * natoms()
QuantisedFrame SortedQuantisedFrame::toQFrame() const
{
    QuantisedFrame result(sorted_frame.size(), m_xquant, m_yquant, m_zquant);
    
    for(int d = 0; d < 3; ++d)
    {
        result.min_coord[d] = min_coord[d];
        result.max_coord[d] = max_coord[d];
    }
    
    for(int i = 0; i < sorted_frame.size(); ++i)
    {
        unsigned int index = sorted_frame[i].first;
        unsigned int pos = sorted_frame[i].second;
        
        int x_bits = 0;
        int y_bits = 0;
        int z_bits = 0;
        
        // Find coordinates from index
        int place = 0;
        unsigned int ix, iy, iz;
        ix = iy = iz = 0;
        
        while(x_bits != m_xquant || y_bits != m_yquant || z_bits != m_zquant)
        {
            if(x_bits != m_xquant)
            {
                int b = (index&(1 << place))>>place;
                ++place;
                ix = ix | (b << x_bits++);
            }
            
            if(y_bits != m_yquant)
            {
                int b = (index&(1 << place))>>place;
                ++place;
                iy = iy | (b << y_bits++);
            }
            
            if(z_bits != m_zquant)
            {
                int b = (index&(1 << place))>>place;
                ++place;
                iz = iz | (b << z_bits++);
            }
        }
        
        result.quantised_frame[3*pos] = ix;
        result.quantised_frame[3*pos+1] = iy;
        result.quantised_frame[3*pos+2] = iz;
    }
    
    return result;
}
        
int SortedQuantisedFrame::natoms() const
{
    /// Everything's been reduced to 1 index in the array
    return sorted_frame.size();
}

