#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/Frame.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char ** argv) {
    assert(argc > 3);
    char * in_path  = argv[1];
    char * out_path = argv[2];

    DCDReader reader;
    reader.open_file(in_path);

    DCDWriter writer;
    writer.save_dcd_file(out_path, reader.natoms());

    Frame frame(reader.natoms());
    for (int i = 3; i < argc; i++) {
        int idx = atoi(argv[i]);
        assert(idx >= 0 && idx < reader.nframes());
        reader.set_frame(idx);
        reader.next_frame(frame);
        writer.save_dcd_frame(frame);
    }

    return 0;
}
