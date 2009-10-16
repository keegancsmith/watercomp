#include "FrameSplitter.h"
#include <vector>
#include <map>
#include <string>
#include "WaterMolecule.h"
#include "../quantiser/QuantisedFrame.h"
#include "../pdbio/AtomInformation.h"

using namespace std;

void split_frame(const vector<AtomInformation> & pdb,
                                vector<WaterMolecule>& water_molecules,
                                vector<unsigned int>& other_atoms)
{
    /// Index of the atom into PDB the file. 0-based.
    /// Maps atom name -> residue name -> residue sequence number -> segment id -> index
    map< string, map < string, map < unsigned int, map < string, unsigned int > > > > lookup_map;
    
    for(size_t i = 0; i < pdb.size(); ++i)
        if(pdb[i].is_water())
            lookup_map[pdb[i].atom_name][pdb[i].residue_name][pdb[i].residue_sequence][pdb[i].seg_id] = pdb[i].index;
        else
            other_atoms.push_back(i);
        
    for(map < string, map < unsigned int, map < string, unsigned int > > >::iterator it1 = lookup_map["OH2"].begin(); it1 != lookup_map["OH2"].end(); ++it1)
    {
        string residue_name = it1->first;
        
        for(map < unsigned int, map < string, unsigned int > >::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
        {
            unsigned int residue_sequence = it2->first;
            
            for(map < string, unsigned int >::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
            {
                string seg_id = it3->first;
                
                unsigned int OH2_index = it3->second;                        
                unsigned int H1_index = lookup_map["H1"][residue_name][residue_sequence][seg_id];
                unsigned int H2_index = lookup_map["H2"][residue_name][residue_sequence][seg_id];
                
                water_molecules.push_back(WaterMolecule(OH2_index, H1_index, H2_index));
            }
        }
    }
}