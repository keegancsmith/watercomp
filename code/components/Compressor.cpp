#include "Compressor.h"

#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/PDBReader.h"
#include "quantiser/QuantisedFrame.h"

#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

#define QUANTISATION 8

bool compress(Compressor & c, string dcdpath, string outpath)
{
    // Start reading actual DCD
    DCDReader reader;
    if (!reader.open_file(dcdpath.c_str()))
        return false;

    // Output file
    FILE *out = fopen(outpath.c_str(), "w");

    // Use the writer
    FrameWriter * writer = c.frame_writer(out);

    // Header
    writer->start(reader.natoms(), reader.nframes());

    // Do each frame
    Frame frame(reader.natoms());
    unsigned int frame_num = 0;
    while(reader.next_frame(frame)) {
        cout << frame_num++ << endl;
        QuantisedFrame qframe(frame, QUANTISATION, QUANTISATION, QUANTISATION);

        // Write out
        writer->next_frame(qframe);
    }

    assert(frame_num == reader.nframes());

    // Clean up
    writer->end();
    delete writer;
    fclose(out);

    return true;
}


bool decompress(Compressor & c, string cmppath, string dcdpath)
{
    // Start reading compressed file
    FILE * fin = fopen(cmppath.c_str(), "r");
    FrameReader * reader = c.frame_reader(fin);

    // Read in header
    reader->start();

    // Output file
    DCDWriter writer;
    writer.save_dcd_file(dcdpath.c_str(), reader->natoms());

    // Read in each frame
    QuantisedFrame qframe(reader->natoms(), 0, 0, 0);
    int frame_num = 0;
    while(reader->next_frame(qframe)) {
        cout << frame_num++ << endl;

        Frame frame = qframe.toFrame();
        writer.save_dcd_frame(frame);
    }

    assert(frame_num == reader->nframes());

    // Clean up
    reader->end();
    delete reader;
    fclose(fin);

    return true;
}


int do_main(Compressor & c, int argc, char ** argv)
{
    assert(argc == 4);
    assert(argv[1][0] == 'c' || argv[1][0] == 'd');

    char   cmd     = argv[1][0];
    string inpath  = argv[2];
    string outpath = argv[3];

    bool success;
    if (cmd == 'c')
        success = compress(c, inpath, outpath);
    else
        success = decompress(c, inpath, outpath);

    return success ? 0 : 1;
}
