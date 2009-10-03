#include <iostream>

#include "NaiveWriter.h"
#include "pdbio/DCDReader.h"
#include "pdbio/PDBReader.h"
#include "quantiser/QuantisedFrame.h"
#include "splitter/FrameSplitter.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

#define QUANTISATION 8

bool compress(const string & dcdpath)
{
    // Load up PDB
    string pdbpath = dcdpath.substr(0, dcdpath.size() - 3) + "pdb";
    vector<AtomInformation> pdb_info = PDBReader::load_pdbfile(pdbpath);

    // Start reading actual DCD
    DCDReader reader;
    if (!reader.open_file(dcdpath.c_str()))
        return false;

    // Output file
    string outpath = dcdpath.substr(0, dcdpath.size() - 3) + "cmp";
    FILE *out = fopen(outpath.c_str(), "w");

    // Use the writer
    NaiveWriter writer(out);
    
    // Header
    writer.start(reader.natoms(), reader.nframes()); 
    
    // Do each frame
    Frame frame(reader.natoms());
    unsigned int frame_num = 0;
    while(reader.next_frame(frame)) {
        cout << frame_num++ << endl;
        QuantisedFrame qframe(frame, QUANTISATION, QUANTISATION, QUANTISATION);
        
        // Write out
        writer.next_frame(qframe);
    }

    assert(frame_num == reader.nframes());

    // Clean up
    writer.end();
    fclose(out);

    return true;
}


int main(int main, char ** argv)
{
    return compress(argv[1]) ? 0 : 1;
}
