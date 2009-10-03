#include "NaiveWriter.h"
#include "NaiveReader.h"
#include "Compressor.h"

class NaiveCompressor : public Compressor
{
public:
    FrameReader * frame_reader(FILE * fin) {
        return new NaiveReader(fin);
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new NaiveWriter(fout);
    }
};


int main(int argc, char ** argv)
{
    NaiveCompressor c;
    return do_main(c, argc, argv);
}
