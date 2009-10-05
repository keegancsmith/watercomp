#include "QuantWriter.h"
#include "QuantReader.h"
#include "Compressor.h"

class QuantCompressor : public Compressor
{
public:
    FrameReader * frame_reader(FILE * fin) {
        return new QuantReader(fin);
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new QuantWriter(fout);
    }
};


int main(int argc, char ** argv)
{
    QuantCompressor c;
    return do_main(c, argc, argv);
}
