#include <iostream>

#include "NaiveWriter.h"
#include "NaiveReader.h"
#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/PDBReader.h"
#include "quantiser/QuantisedFrame.h"
#include "splitter/FrameSplitter.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

#define QUANTISATION 8

bool compress(string dcdpath, string outpath)
{
    // Start reading actual DCD
    DCDReader reader;
    if (!reader.open_file(dcdpath.c_str()))
        return false;

    // Output file
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


bool decompress(string cmppath, string dcdpath) {
    // Start reading compressed file
    FILE * fin = fopen(cmppath.c_str(), "r");
    NaiveReader reader(fin);

    // Read in header
    reader.start();

    // Output file
    DCDWriter writer;
    writer.save_dcd_file(dcdpath.c_str(), reader.natoms());

    // Read in each frame
    QuantisedFrame qframe(reader.natoms(), 0, 0, 0);
    int frame_num = 0;
    while(reader.next_frame(qframe)) {
        cout << frame_num++ << endl;

        Frame frame = qframe.toFrame();
        writer.save_dcd_frame(frame);
    }

    assert(frame_num == reader.nframes());

    // Clean up
    reader.end();
    fclose(fin);

    return true;
}


int main(int argc, char ** argv)
{
    assert(argc == 4);
    assert(argv[1][0] == 'c' || argv[1][0] == 'd');

    char   cmd     = argv[1][0];
    string inpath  = argv[2];
    string outpath = argv[3];

    bool success;
    if (cmd == 'c')
        success = compress(inpath, outpath);
    else
        success = decompress(inpath, outpath);

    return success ? 0 : 1;
}
