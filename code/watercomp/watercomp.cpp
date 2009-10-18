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
        return new WaterReader(fin, m_atom_information);
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new WaterWriter(fout, m_atom_information);
    }

    bool needs_atom_information() const { return true; }
};


int main(int argc, char ** argv)
{
    WaterCompressor c;
    return do_main(c, argc, argv);
}
