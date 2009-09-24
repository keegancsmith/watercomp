#include "QFrame.h"
#include <cassert>

using namespace std;

QFrame::QFrame(const Frame & frame,
               unsigned int x_subs, unsigned int y_subs, unsigned int z_subs)
    : m_xquant(x_subs), m_yquant(y_subs), m_zquant(z_subs)
{
    for(int d = 0; d < 3; ++d)
        min_coord[d] = max_coord[d] = frame.atom_data[d];
        
    for(size_t i = 1; i < frame.natoms; ++i) 
        for(int d = 0; d < 3; ++d) 
        {
            min_coord[d] = min(frame.atom_data[3*i + d], min_coord[d]);
            max_coord[d] = max(frame.atom_data[3*i + d], max_coord[d]);
        }
        
    float range[3];
    
    for(int d = 0; d < 3; ++d)
        range[d] = max_coord[d] - min_coord[d];
    
    unsigned int buckets[3] = {1 << x_subs, 1 << y_subs, 1 << z_subs};
    
    for(size_t i = 0; i < frame.natoms; ++i)
        for(int d = 0; d < 3; ++d)
        {
            float translated = frame.atom_data[3*i + d] - min_coord[d];
            float scaled = translated * buckets[d] / range[d];
            
            if(scaled < 0.5)
                scaled = 0.5;
            else if(scaled >= buckets[d] - 0.5)
                scaled = buckets[d] - 0.5;
            
            quantised_frame.push_back(scaled);
            
            assert(0 <= scaled && scaled <= buckets[d]-1);
        }

    atom_information = frame.atom_information;
}

int QFrame::natoms() const {
    return (int) quantised_frame.size();
}


Frame QFrame::toFrame(float * atom_data) const {
    unsigned int buckets[3] = {1 << m_xquant, 1 << m_yquant, 1 << m_zquant};
    float range[3];
    for (int d = 0; d < 3; d++)
        range[d] = max_coord[d] - min_coord[d];


    // Approximate quantised points with floats
    for (int j = 0; j < natoms(); j++) {        
        for (int d = 0; d < 3; d++) {
            int i = j * 3 + d;
            float approx = quantised_frame[i] + 0.5;
            atom_data[i] = approx * range[d] / buckets[d] + min_coord[d];
        }
    }

    return Frame(atom_data, quantised_frame.size(), atom_information);
}
