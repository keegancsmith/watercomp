#include "GumholdWriter.h"
#include "GumholdReader.h"
#include "Compressor.h"
#include "TreeSerialiser.h"

#include <cassert>
#include <cstdlib>
#include <string>


void gumhold_constant_predictor(const QuantisedFrame * frame, int v,
                                int v_parent, unsigned int * p) {
    if (v == -1) {
        // root is predicted to be at (0,0,0)
        p[0] = p[1] = p[2] = 0;
        return;
    }

    for (int i = 0; i < 3; i++)
        p[i] = frame->quantised_frame[3*v + i];
}


void gumhold_linear_predictor(const QuantisedFrame * frame, int v,
                              int v_parent, unsigned int * p) {
    if (v_parent == -1) {
        gumhold_constant_predictor(frame, v, v_parent, p);
        return;
    }

    for (int d = 0; d < 3; d++) {
        int a = frame->quantised_frame[3*v + d];
        int b = frame->quantised_frame[3*v_parent + d];
        int prediction = a + (a - b);
        if (prediction < 0)
            prediction = 0;
        p[d] = prediction;
    }
}


std::string get_predictor_env()
{
    char * perm = getenv("PREDICTOR");
    if (!perm)
        return "constant";
    else return perm;
}

const char * pred_error_msg = "ERROR: PREDICTOR environment variable must "
    "be 'linear' or 'constant' (default)\n";

gumhold_predictor * get_gumhold_predictor()
{
    std::string perm = get_predictor_env();
    if (perm == "constant")
        return gumhold_constant_predictor;
// TODO make grid work with linear
//    if (perm == "linear")
//        return gumhold_linear_predictor;
    fprintf(stderr, "%s", pred_error_msg);
    exit(1);
}


class GumholdCompressor : public Compressor
{
public:
    FrameReader * frame_reader(FILE * fin) {
        return new GumholdReader(fin, this, get_gumhold_predictor());
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new GumholdWriter(fout, this, get_gumhold_predictor());
    }

    bool needs_permutation_compressor() const { return true; }
};


int main(int argc, char ** argv)
{
    GumholdCompressor c;
    return do_main(c, argc, argv);
}
