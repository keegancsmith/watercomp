#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/Frame.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
    assert(argc == 4);
    char * in_path  = argv[1];
    char * out_path = argv[2];
    int natoms      = atoi(argv[3]);

    DCDReader reader;
    reader.open_file(in_path);
    assert(0 < natoms && natoms <= reader.natoms());

    DCDWriter writer;
    writer.save_dcd_file(out_path, natoms);

    cout << "Taking first " << natoms << " of " << reader.natoms() << endl;

    Frame frame_bigger(reader.natoms());
    Frame frame_smaller(natoms);
    for (int i = 0; i < reader.nframes(); i++) {
        reader.next_frame(frame_bigger);
        copy(frame_bigger.atom_data.begin(),
             frame_bigger.atom_data.begin() + (natoms * 3),
             frame_smaller.atom_data.begin());
        writer.save_dcd_frame(frame_smaller);
    }

    return 0;
}
