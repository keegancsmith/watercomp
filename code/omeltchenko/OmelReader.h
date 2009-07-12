#pragma once
#include <cstdio>
#include <queue>
#include <vector>
#include "Atom.h"

class OmelReader
{
    public:
        /* Constructs a Omeltchenko reader object to read from the specified file. */
        OmelReader(const char* filename, int in_initial_l = 3, int in_delta_l = 2, int in_max_adapt_initial_l = 32, int in_max_adapt_delta_l = 32);
        
        /* Starts the reading process. */
        void start_read();
        
        /* Ends the reading process. */
        void end_read();
        
        /* Reads a frame. Returns false if no more frames */
        bool read_frame(std::vector<Atom>& atom_list);
        
        /* Accessor for number of atoms */
        int get_atoms() { return atoms; };
        
        /* Accessor for frames written */
        int get_frames() { return frames_left; };
        
        /* Accessors for dcd header data */
        int get_istart() { return istart; };
        int get_nsavc() { return nsavc; };
        double get_delta() { return delta; };
        
    private:
        /* File required for input */
        FILE* in_file;
        
        /* Reads an unsigned 32-bit integer */
        unsigned int read_uncompressed_int();
        
        /* Reads an compressed unsigned 32-bit integer */
        unsigned int read_compressed_int();
        
        /* Reads a bit from the stream */
        bool get_bit();
        
        /* Gets the number of bits required to represent the unsigned 32-bit integer */
        int num_bits(unsigned int num);
        
        /* A bit buffer for storing bits read, but not used */
        std::queue<bool> bit_buffer;
        
        /* Number of frames */
        int frames_left;
        
        /* Number of atoms in each frame */
        int atoms;
        
        /* The number of bits initially allocated to storing a number */
        int initial_l;
        
        /* The number of extra bits allocated to storing a number */
        int delta_l;
        
        /* The current status of adapting the initial allocation of bits */
        int adapt_initial_l;
        
        /* The current status of adapting the extra allocation of bits */
        int adapt_delta_l;
        
        /* Maximum adapt_initial_l until initial_l is changed */
        int max_adapt_initial_l;
        
        /* Maximum adapt_initial_l until delta_l is changed */
        int max_adapt_delta_l;
        
        /* Data from the DCD file header */
        int istart;
        int nsavc;
        double delta;
};