#include "GumholdWriter.h"
#include "GumholdReader.h"
#include "Compressor.h"

class GumholdCompressor : public Compressor
{
public:
    FrameReader * frame_reader(FILE * fin) {
        return new GumholdReader(fin);
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new GumholdWriter(fout);
    }
};


int main(int argc, char ** argv)
{
    GumholdCompressor c;
    return do_main(c, argc, argv);
}
