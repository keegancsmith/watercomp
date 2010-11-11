#include "WaterWriter.h"
#include "WaterReader.h"
#include "Compressor.h"

#include <cassert>
#include <cstdlib>
#include <string>


class WaterCompressor : public Compressor
{
public:
    FrameReader * frame_reader(FILE * fin) {
        return new WaterReader(fin, this);
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new WaterWriter(fout, this);
    }

    bool needs_atom_information() const { return true; }
    bool needs_permutation_compressor() const { return true; }
};


int main(int argc, char ** argv)
{
    WaterCompressor c;
    return do_main(c, argc, argv);
}
