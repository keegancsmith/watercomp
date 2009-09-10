#pragma once

#include <vector>
#include "WaterMolecule.h"
#include "../quantiser/QFrame.h"

class FrameSplitter
{
    public:
        static void split_frame(QFrame& input_atoms, std::vector<WaterMolecule>&  water_molecules, QFrame& other_atoms)
        {
            for(size_t i = 0; i < input_atoms.quantised_frame.size(); i+=3)
                if((*input_atoms.atom_information)[i].is_water())
                {
                    /// EPIC HACK - TO FIX WE ACTUALLY NEED TO CLEVERLY EXTRACT WATERS ON A GLOBAL SCALE
                    /// Or re-order the PDB to fit this scheme - which is best.
                    unsigned int* atom_index = &input_atoms.quantised_frame[i];
                    
                    water_molecules.push_back(WaterMolecule(atom_index, atom_index+3, atom_index+6));
                    i+=6;
                }
                else
                {
                    unsigned int* atom_index = &input_atoms.quantised_frame[i];
                    
                    for(int d = 0; d < 3; ++d)
                        other_atoms.quantised_frame.push_back(atom_index[d]);
                }
        }
};