#include <vector>
#include <string>

class AtomInformation
{
    public:
        AtomInformation(unsigned int n_index, std::string text);
        
        /// Index of the atom into PDB the file. 0-based.
        unsigned int index;
        
        /// What is stored here is only molecule information, no position data,
        /// Four items below serve as a four tuple of the primary key.
        
        /// Name of the atom: N, C, O, etc; OH2, H1, H2 signify water atoms
        std::string atom_name;
        
        /// Residue Name: MET(methane?!?), etc: Water appears to be LEU3 only
        std::string residue_name;
        
        /// Residue Sequence Number: A number representing the number of molecule
        /// this atom belongs to. IE: The first water molecule in the residue is
        /// 1...
        unsigned int residue_sequence;
        
        /// Segment Identifier: This serves to delimit various atoms into groups.
        /// Waters tend to have the format: W[APT][1234].
        std::string seg_id;
};