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
        printf("Processing Frame %d of %d...", i+1, dcd->nsets);

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
    delete[] timestep.coords;
}


void decompress(string in_path, string out_path) {
    ac_decoder decoder;
    ac_model model;
    FILE * fin;
    float * points[3];


    // Setup arithmetic decoders to use in_path
    fin = fopen(in_path.c_str(), "rb");
    ac_decoder_init(&decoder, fin);
    ac_model_init(&model, 256, NULL, 1);


    // Read global header
    int header[3];
    ac_decode_data(header, sizeof(int), 3, &decoder, &model);
    int quantisation_bits = header[0];
    int natoms            = header[1];
    int nframes           = header[2];


    // dcd output
    fio_fd fd;
    int    istart        = 0;
    int    nsavc         = 1;
    double delta         = 1.0;
    int    with_unitcell = 0;
    int    charmm        = 1;
    assert(fio_open(out_path.c_str(), FIO_WRITE, &fd) >= 0);
    write_dcdheader(fd, "Created by Devillers & Gandoin Compressor.", natoms,
                    istart, nsavc, delta, with_unitcell, charmm);


    // Points buffer. Need to store each dimension seperately
    for (int d = 0; d < 3; d++)
        points[d] = new float[natoms];


    // Process each frame and write out
    for (int i = 0; i < nframes; ++i)
    {
        printf("Processing Frame %d of %d...", i+1, nframes);

        // Read frame header
        float min_box[3], max_box[3];
        unsigned int size;
        ac_decode_data(min_box, sizeof(float), 3, &decoder, &model);
        ac_decode_data(max_box, sizeof(float), 3, &decoder, &model);
        ac_decode_data(&size,   sizeof(unsigned int), 1, &decoder, &model);

        float range[3];
        for (int d = 0; d < 3; d++)
            range[d] = max_box[d] - min_box[d];


        // TODO read in points permutation


        // Read in encoded data
        vector<coord_t> data;
        for (size_t j = 0; j < size; j++) {
            coord_t c;
            ac_decode_data(&c, sizeof(coord_t), 1, &decoder, &model);
            data.push_back(c);
        }


        // Do the reverse devillers and gandoin transformation
        vector<point_t> decoded = decode(data, quantisation_bits);


        // Approximate quantised points with floats
        for (size_t j = 0; j < decoded.size(); j++) {
            float buckets = 1 << quantisation_bits;

            for (int d = 0; d < 3; d++) {
                float approx = decoded[j].coords[d] + 0.5;
                points[d][j] = approx * range[d] / buckets;
            }
        }


        // Write uncompressed atoms
        write_dcdstep(fd, i+1, istart + nsavc*(i+1), natoms,
                      points[0], points[1], points[3], NULL, 1);


        printf("Done\n");
    }


    // Cleanup
    ac_decoder_done(&decoder);
    ac_model_done(&model);
    fclose(fin);
    //fio_close(fd); // Won't compile for some reason
    for (int d = 0; d < 3; d++)
        delete[] points[d];
}


int usage() {
    printf("USAGE: ./dcd (c|d) in out\n");
}


int main(int argc, char ** argv) {
    assert(argc == 4);

    string cmd = argv[1];

    if (cmd == "c")
        compress(argv[2], argv[3]);
    else if (cmd == "d")
        decompress(argv[2], argv[3]);
    else {
        usage();
        return 1;
    }

    return 0;
}
