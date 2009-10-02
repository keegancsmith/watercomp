#include "AtomInformation.h"
#include <cstdio>

using namespace std;

AtomInformation::AtomInformation(unsigned int n_index, string text)
{
    index = n_index;
    
    char buffer[5];
    
    sscanf(text.c_str()+12, "%4s", buffer);
    atom_name = buffer;
    
    sscanf(text.c_str()+17, "%3s", buffer);
    residue_name = buffer;
    
    sscanf(text.c_str()+22, "%4u", &residue_sequence);
    
    sscanf(text.c_str()+72, "%4s", buffer);
    seg_id = buffer;
}

bool AtomInformation::is_water() const
{
    return atom_name == "OH2" || atom_name == "H1" || atom_name == "H2";
}
