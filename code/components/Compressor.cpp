#include "Compressor.h"

#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/PDBReader.h"
#include "quantiser/QuantisedFrame.h"

extern "C" {
#include <getopt.h>
}

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;

#define QUANTISATION 8
#define MAXQUANT 64
#define MINQUANT 4


bool compress(Compressor & c, string dcdpath, string outpath,
              int quantx, int quanty, int quantz)
{
    // Start reading actual DCD
    DCDReader reader;
    if (!reader.open_file(dcdpath.c_str()))
        return false;

    // Output file
    FILE *out = fopen(outpath.c_str(), "w");

    // Use the writer
    FrameWriter * writer = c.frame_writer(out);

    // Header
    writer->start(reader.natoms(), reader.nframes());

    // Do each frame
    Frame frame(reader.natoms());
    unsigned int frame_num = 0;
    while(reader.next_frame(frame)) {
        cout << frame_num++ << endl;
        QuantisedFrame qframe(frame, quantx, quanty, quantz);

        // Write out
        writer->next_frame(qframe);
    }

    assert(frame_num == reader.nframes());

    // Clean up
    writer->end();
    delete writer;
    fclose(out);

    return true;
}


bool decompress(Compressor & c, string cmppath, string dcdpath)
{
    // Start reading compressed file
    FILE * fin = fopen(cmppath.c_str(), "r");
    FrameReader * reader = c.frame_reader(fin);

    // Read in header
    reader->start();

    // Output file
    DCDWriter writer;
    writer.save_dcd_file(dcdpath.c_str(), reader->natoms());

    // Read in each frame
    QuantisedFrame qframe(reader->natoms(), 0, 0, 0);
    int frame_num = 0;
    while(reader->next_frame(qframe)) {
        cout << frame_num++ << endl;

        Frame frame = qframe.toFrame();
        writer.save_dcd_frame(frame);
    }

    assert(frame_num == reader->nframes());

    // Clean up
    reader->end();
    delete reader;
    fclose(fin);

    return true;
}


int parse_quant(const char * s) {
    int i = atoi(s);
    if (i < MINQUANT || i > MAXQUANT) {
        fprintf(stderr, "Quantisation values must be integers in "
                "the range %d to %d.\n", MINQUANT, MAXQUANT);
        exit(1);
    }
    return i;
}


void usage(char * prog) {
    fprintf(stderr, "USAGE: %s (-c|-d) [-q] [-x] [-y] [-z]"
            " input_path output_path\n", prog);

    fprintf(stderr, "\n"
            "You must specify one of --compress (-c) or --decompress (-d).\n"
            "Quantisation defaults to %d bits.\n\n"
            
            "\t-q b\n\t--quantisation b "
            "Set the quantisation in all dimensions to b bits.\n\n"

            "\t-x b\n\t--quantx b\t"
            "Set the quantisation in the x dimension to b bits.\n\n"

            "\t-y b\n\t--quanty b\t"
            "Set the quantisation in the y dimension to b bits.\n\n"

            "\t-z b\n\t--quantz b\t"
            "Set the quantisation in the z dimension to b bits.\n",
            QUANTISATION);
}


int do_main(Compressor & c, int argc, char ** argv)
{
    // Arguments
    bool docompress   = false;
    bool dodecompress = false;
    int quantx = QUANTISATION;
    int quanty = QUANTISATION;
    int quantz = QUANTISATION;
    char *input_path;
    char *output_path;


    // Parse arguments
    while (true) {
        struct option long_options[] = {
            // Flags
            {"compress",     no_argument, 0, 'c'},
            {"decompress",   no_argument, 0, 'd'},
            {"help",         no_argument, 0, 'h'},
            // Options
            {"quantisation", required_argument, 0, 'q'},
            {"quantx",       required_argument, 0, 'x'},
            {"quanty",       required_argument, 0, 'y'},
            {"quantz",       required_argument, 0, 'z'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "cdq:x:y:z:",
                            long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 'c':
            docompress = true;
            break;

        case 'd':
            dodecompress = true;
            break;

        case 'q':
            quantx = quanty = quantz = parse_quant(optarg);
            break;

        case 'x':
            quantx = parse_quant(optarg);
            break;

        case 'y':
            quanty = parse_quant(optarg);
            break;

        case 'z':
            quantz = parse_quant(optarg);
            break;

        case 'h':
            usage(argv[0]);
            return 0;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort();
        }
    }


    // Error reporting
    if (optind + 2 != argc) {
        fprintf(stderr, "ERROR: Please specify an input and output path.\n\n");
        usage(argv[0]);
        return 1;
    } else if (docompress == dodecompress) {
        fprintf(stderr, "ERROR: Please specify --compress or --decompress.\n\n");
        usage(argv[0]);
        return 1;
    }


    input_path  = argv[optind];
    output_path = argv[optind+1];


    // Run compressor or decompressor
    bool success;
    if (docompress)
        success = compress(c, input_path, output_path,
                           quantx, quanty, quantz);
    else
        success = decompress(c, input_path, output_path);

    return success ? 0 : 1;
}
