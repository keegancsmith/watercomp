#include "QFrame.h"
#include <cassert>

using namespace std;

QFrame::QFrame(const Frame & frame,
               unsigned int x_subs, unsigned int y_subs, unsigned int z_subs)
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
        range[d] = max_coord[d] - max_coord[d];
    
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
