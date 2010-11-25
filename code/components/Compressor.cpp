#include "Compressor.h"
#include "Permutation.h"

#include "pdbio/DCDReader.h"
#include "pdbio/DCDWriter.h"
#include "pdbio/PDBReader.h"
#include "quantiser/QuantisedFrame.h"

extern "C" {
#include <getopt.h>
}

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <iostream>
#include <string>

using namespace std;

#define QUANTISATION 8
#define MAXQUANT 64
#define MINQUANT 4


void print_error(const char *error, ...)
{
    va_list ap;

    fprintf(stderr, "ERROR: ");

    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);

    fprintf(stderr, "\nRun with --help for usage.\n");

    exit(1);
}


string progress_bar(int count, int size)
{
    string bar = "[                                                  ] |";
    int nhashes = count * 50 / size;
    for (int i = 1; i <= nhashes; i++)
        bar[i] = '#';
    bar[53] = "|/-\\"[count % 4];
    return bar;
}


bool compress(Compressor & c, string dcdpath, string outpath,
              int quantx, int quanty, int quantz)
{
    // Start reading actual DCD
    DCDReader reader;
    if (!reader.open_file(dcdpath.c_str()))
        return false;

    // Output file
    FILE *out = fopen(outpath.c_str(), "wb");

    // Use the writer
    FrameWriter * writer = c.frame_writer(out);

    // Header
    writer->start(reader.natoms(), reader.nframes());

    // Keep track of the bounding box
    float min_coord[3] = { std::numeric_limits<float>::max() };
    float max_coord[3] = { std::numeric_limits<float>::min() };

    // Do each frame
    Frame frame(reader.natoms());
    unsigned int frame_num = 0;
    while(reader.next_frame(frame)) {
        cout << '\r' << progress_bar(frame_num++, reader.nframes()) << flush;
        QuantisedFrame qframe(frame, quantx, quanty, quantz,
                              min_coord, max_coord);

        for (int d = 0; d < 3; d++) {
            min_coord[d] = qframe.min_coord[d] = min(min_coord[d],
                                                     qframe.min_coord[d]);
            max_coord[d] = qframe.max_coord[d] = max(max_coord[d],
                                                     qframe.max_coord[d]);
        }

        // Write out
        writer->next_frame(qframe);
    }
    cout << '\r' << progress_bar(frame_num, reader.nframes()) << endl;

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
    FILE * fin = fopen(cmppath.c_str(), "rb");
    FrameReader * reader = c.frame_reader(fin);

    // Read in header
    reader->start();

    // Output file
    DCDWriter writer;
    writer.save_dcd_file(dcdpath.c_str(), reader->natoms());

    // Read in each frame
    QuantisedFrame qframe(reader->natoms(), 0, 0, 0);
    for (int frame_num = 0; frame_num < reader->nframes(); frame_num++) {
        cout << '\r' << progress_bar(frame_num, reader->nframes()) << flush;
        reader->next_frame(qframe);
        Frame frame = qframe.toFrame();
        writer.save_dcd_frame(frame);
    }
    cout << '\r' << progress_bar(1, 1) << endl;


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

            "\t-p path\n\t--pdb path "
            "Set the path of the pdb file.\n\n"

            "\t-t perm_compressor\n\t--permutation perm_compressor "
            "Set the permutation compressor to use. Pass help to see the "
            "list of permutation compressors.\n\n"

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


string check_perm_compressor(string perm) {
    if (perm.size() == 0)
        return permutation_compressors[0];

    for (int i = 0; permutation_compressors[i] != ""; i++)
        if (perm == permutation_compressors[i])
            return perm;

    if (perm != "help")
        fprintf(stderr, "ERROR: ");

    fprintf(stderr, "The permutation compressor must be one of: ");
    fprintf(stderr, "'%s' (default)", permutation_compressors[0].c_str());
    for (int i = 1; permutation_compressors[i] != ""; i++)
        fprintf(stderr, ", '%s'", permutation_compressors[i].c_str());
    fprintf(stderr, "\n");

    exit(perm == "help" ? 0 : 1);
}


bool file_is_readable(const string & path)
{
    ifstream fin(path.c_str());
    bool readable = fin.good();
    fin.close();
    return readable;
}


int do_main(Compressor & c, int argc, char ** argv)
{
    // Arguments
    bool docompress   = false;
    bool dodecompress = false;
    int quantx = QUANTISATION;
    int quanty = QUANTISATION;
    int quantz = QUANTISATION;
    string pdb_path;
    string perm_compressor;
    string input_path;
    string output_path;


    // Parse arguments
    while (true) {
        struct option long_options[] = {
            // Flags
            {"compress",     no_argument, 0, 'c'},
            {"decompress",   no_argument, 0, 'd'},
            {"help",         no_argument, 0, 'h'},
            // Options
            {"pdb",          required_argument, 0, 'p'},
            {"permutation",  required_argument, 0, 't'},
            {"quantisation", required_argument, 0, 'q'},
            {"quantx",       required_argument, 0, 'x'},
            {"quanty",       required_argument, 0, 'y'},
            {"quantz",       required_argument, 0, 'z'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "cdp:t:q:x:y:z:",
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

        case 'p':
            pdb_path = optarg;
            break;

        case 't':
            perm_compressor = optarg;
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
    if (optind + 2 != argc)
        print_error("Please specify an input and output path.");
    if (docompress == dodecompress)
        print_error("Please specify --compress or --decompress.");


    // Input and output are the remaining arguments
    input_path  = argv[optind];
    output_path = argv[optind+1];

    if (!file_is_readable(input_path))
        print_error("Could not read '%s'.", input_path.c_str());


    if (c.needs_permutation_compressor())
        c.m_permutation_compressor = check_perm_compressor(perm_compressor);
    bool perm_compressor_needs_pdb = c.m_permutation_compressor == "pdb";


    // Check if we need to read in PDB info
    if (c.needs_atom_information() || perm_compressor_needs_pdb) {
        // Guess pdb_path if it is not set
        if (pdb_path.size() == 0) {
            size_t dot_pos = input_path.rfind('.');
            if (dot_pos == string::npos)
                dot_pos = input_path.size();

            pdb_path = input_path.substr(0, dot_pos) + ".pdb";
        }

        if (!file_is_readable(pdb_path))
            print_error("Could not read '%s'. Please specify "
                        "the pdb file with --pdb\n\n", pdb_path.c_str());

        c.m_atom_information = load_pdbfile(pdb_path);
    }


    // Run compressor or decompressor
    bool success;
    if (docompress)
        success = compress(c, input_path, output_path,
                           quantx, quanty, quantz);
    else
        success = decompress(c, input_path, output_path);

    return success ? 0 : 1;
}
