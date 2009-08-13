#include "../dcd_loader/dcdplugin.h"
#include "ac.h"
#include "devillersgandoin.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;


void ac_encode_data(const void *ptr, size_t size, size_t nmemb,
                    ac_encoder *encoder, ac_model *model) {
    size_t n = size * nmemb;
    for (size_t i = 0; i < n; i++)
        ac_encode_symbol(encoder, model, ((const unsigned char*)ptr)[i]);
}


void ac_decode_data(void *ptr, size_t size, size_t nmemb,
                    ac_decoder *decoder, ac_model *model) {
    size_t n = size * nmemb;
    for (size_t i = 0; i < n; i++)
        ((unsigned char*)ptr)[i] = ac_decode_symbol(decoder, model);
}


void compress(string dcd_path, string out_path, int quantisation_bits=10) {
    ac_encoder encoder;
    ac_model model;
    int natoms;
    FILE * fout;
    molfile_timestep_t timestep;


    // Load dcd file using dcd library
    dcdhandle* dcd = (dcdhandle*) open_dcd_read(dcd_path.c_str(), "dcd", &natoms);
    timestep.coords = new float[3*natoms];


    // Setup arithmetic encoders to use out_path
    fout = fopen(out_path.c_str(), "wb");
    ac_encoder_init(&encoder, fout);
    ac_model_init(&model, 256, NULL, 1);


    // Write global header
    int header[3] = {quantisation_bits, natoms, dcd->nsets};
    ac_encode_data(header, sizeof(int), 3, &encoder, &model);


    // Process each frame and write out
    for (int i = 0; i < dcd->nsets; ++i)
    {
        assert(!read_next_timestep(dcd, natoms, &timestep));
        printf("Processing Frame %d of %d...", i, dcd->nsets);

        // Bounding box
        float min_box[3], max_box[3];
        for (int d = 0; d < 3; d++)
            min_box[d] = max_box[d] = timestep.coords[d];
        for (int j = 1; j < natoms; ++j) {
            for (int d = 0; d < 3; d++) {
                min_box[d] = min(timestep.coords[3 * j + d], min_box[d]);
                max_box[d] = max(timestep.coords[3 * j + d], max_box[d]);
            }
        }

        float range[3];
        for (int d = 0; d < 3; d++)
            range[d] = max_box[d] - min_box[d];

        // Read in and quantise frame
        vector<point_t> frame;
        for (int j = 0; j < natoms; j++) {
            point_t p;

            float buckets = 1 << quantisation_bits;
            for (int d = 0; d < 3; d++) {
                float scaled = timestep.coords[3*j+d] * buckets / range[d];
                if (scaled < 0.5)
                    scaled = 0.5;
                else if (scaled >= buckets - 0.5)
                    scaled = buckets - 0.5;

                p.coords[d] = (unsigned int) scaled;

                assert(p.coords[d] < (1 << quantisation_bits));
            }

            frame.push_back(p);
        }


        // Do the devillers and gandoin transformation
        vector<coord_t> encoded = encode(frame, quantisation_bits);
        unsigned int size = encoded.size();


        // Write the frame header
        ac_encode_data(min_box, sizeof(float), 3, &encoder, &model);
        ac_encode_data(max_box, sizeof(float), 3, &encoder, &model);
        ac_encode_data(&size,   sizeof(unsigned int), 1, &encoder, &model);

        // TODO Encode points permutation

        // Write encoded points
        for (size_t j = 0; j < size; j++) {
            coord_t c = encoded[j];
            ac_encode_data(&c, sizeof(coord_t), 1, &encoder, &model);
        }

        printf("Done\n");
    }


    // Cleanup
    ac_encoder_done(&encoder);
    ac_model_done(&model);
    fclose(fout);
}


int main(int argc, char ** argv) {
    assert(argc == 3);
    compress(argv[1], argv[2]);
    return 0;
}
