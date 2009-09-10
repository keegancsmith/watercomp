#include "QFrame.h"
#include <cassert>
#include "../pdb_reader/AtomInformation.h"

using namespace std;

QFrame::QFrame(float* atom_data, size_t atoms, vector<AtomInformation>* n_atom_information, unsigned int x_subs, unsigned int y_subs, unsigned int z_subs)
{
    for(int d = 0; d < 3; ++d)
        min_coord[d] = max_coord[d] = atom_data[d];
        
    for(size_t i = 1; i < atoms; ++i) 
        for(int d = 0; d < 3; ++d) 
        {
            min_coord[d] = min(atom_data[3*i + d], min_coord[d]);
            max_coord[d] = max(atom_data[3*i + d], max_coord[d]);
        }
        
    float range[3];
    
    for(int d = 0; d < 3; ++d)
        range[d] = max_coord[d] - max_coord[d];
    
    unsigned int buckets[3] = {1 << x_subs, 1 << y_subs, 1 << z_subs};
    
    for(size_t i = 0; i < atoms; ++i)
        for(int d = 0; d < 3; ++d)
        {
            float scaled = (atom_data[3*i + d] - min_coord[d])*buckets[d]/range[d];
            
            if(scaled < 0.5)
                scaled = 0.5;
            else if(scaled >= buckets[d] - 0.5)
                scaled = buckets[d] - 0.5;
            
            quantised_frame.push_back(scaled);
            
            assert(0 <= scaled && scaled <= buckets[d]-1);
        }

    atom_information = n_atom_information;
}
