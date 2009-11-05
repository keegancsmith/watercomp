#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/Frame.h"
#include "pdbio/PDBReader.h"
#include "splitter/FrameSplitter.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

void metadata(string input_path) {
    DCDReader reader;
    reader.open_file(input_path.c_str());

    size_t dot_pos  = input_path.rfind('.');
    string pdb_path = input_path.substr(0, dot_pos) + ".pdb";
    vector<AtomInformation> pdb = load_pdbfile(pdb_path);

    vector<WaterMolecule> water_molecules;
    vector<unsigned int> other_atoms;
    split_frame(pdb, water_molecules, other_atoms);

    float nwaters = (reader.natoms() - other_atoms.size());
    float water_per = 100 * nwaters / reader.natoms();

    cout << "Path:\t" << input_path << endl
         << "Atoms:\t" << reader.natoms() << endl
         << "Frames:\t" << reader.nframes() << endl
         << "Water:\t" << water_molecules.size() << endl
         << "Water%:\t" << water_per << endl;
}

int main(int argc, char ** argv) {
    for (int i = 1; i < argc; i++) {
        metadata(argv[i]);
        cout << endl;
    }
        
    return 0;
}
