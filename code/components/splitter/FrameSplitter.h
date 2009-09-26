#pragma once

#include <vector>
#include <map>
#include <string>
#include "WaterMolecule.h"
#include "QFrame.h"
#include "AtomInformation.h"

class FrameSplitter
{
    public:
        static void split_frame(std::vector<AtomInformation> pdb, std::vector<WaterMolecule>&  water_molecules, std::vector<unsigned int>& other_atoms)
        {
            /// Index of the atom into PDB the file. 0-based.
//             unsigned int index;
//             
//             /// What is stored here is only molecule information, no position data,
//             /// Four items below serve as a four tuple of the primary key.
//             
//             /// Name of the atom: N, C, O, etc; OH2, H1, H2 signify water atoms
//             std::string atom_name;
//             
//             /// Residue Name: MET(methane?!?), etc: Water appears to be LEU3 only
//             std::string residue_name;
//             
//             /// Residue Sequence Number: A number representing the number of molecule
//             /// this atom belongs to. IE: The first water molecule in the residue is
//             /// 1...
//             unsigned int residue_sequence;
//             
//             /// Segment Identifier: This serves to delimit various atoms into groups.
//             /// Waters tend to have the format: W[APT][1234].
//             std::string seg_id;
            
            /// Maps atom name -> residue name -> residue sequence number -> segment id -> index
            std::map< std::string, std::map < std::string, std::map < unsigned int, std::map < std::string, unsigned int > > > > lookup_map;
            
            for(size_t i = 0; i < pdb.size(); ++i)
                if(pdb[i].is_water())
                    lookup_map[pdb[i].atom_name][pdb[i].residue_name][pdb[i].residue_sequence][pdb[i].seg_id] = pdb[i].index;
                else
                    other_atoms.push_back(i);
                
            for(std::map < std::string, std::map < unsigned int, std::map < std::string, unsigned int > > >::iterator it1 = lookup_map["OH2"].begin(); it1 != lookup_map["OH2"].end(); ++it1)
            {
                std::string residue_name = it1->first;
                
                for(std::map < unsigned int, std::map < std::string, unsigned int > >::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
                {
                    unsigned int residue_sequence = it2->first;
                    
                    for(std::map < std::string, unsigned int >::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
                    {
                        std::string seg_id = it3->first;
                        
                        unsigned int OH2_index = it3->second;                        
                        unsigned int H1_index = lookup_map["H1"][residue_name][residue_sequence][seg_id];
                        unsigned int H2_index = lookup_map["H2"][residue_name][residue_sequence][seg_id];
                     
                        water_molecules.push_back(WaterMolecule(OH2_index, H1_index, H2_index));
                    }
                }
            }
//             for(size_t i = 0; i < input_atoms.quantised_frame.size(); i+=3)
//             {
// //                 printf("%u %u\n", i, input_atoms.quantised_frame.size());
//                 if((*input_atoms.atom_information)[i/3].is_water())
//                 {
// //                     printf("A\n");
//                     /// EPIC HACK - TO FIX WE ACTUALLY NEED TO CLEVERLY EXTRACT WATERS ON A GLOBAL SCALE
//                     /// Or re-order the PDB to fit this scheme - which is best.
//                     unsigned int* atom_index = &input_atoms.quantised_frame[i];
//                     
//                     water_molecules.push_back(WaterMolecule(atom_index, atom_index+3, atom_index+6));
//                     i+=6;
//                 }
//                 else
//                 {
// //                     printf("B\n");
//                     unsigned int* atom_index = &input_atoms.quantised_frame[i];
//                     
//                     for(int d = 0; d < 3; ++d)
//                         other_atoms.quantised_frame.push_back(atom_index[d]);
//                 }
//             }
        }
};