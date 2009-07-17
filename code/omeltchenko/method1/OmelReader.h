#pragma once
#include <cstdio>
#include <queue>
#include <vector>
#include "Atom.h"
#include "OmelDecoder.h"

class OmelReader
{
    public:
        /* Constructs a Omeltchenko reader object to read from the specified file. */
        OmelReader(const char* filename, int in_initial_l = 3, int in_delta_l = 2, int in_max_adapt_initial_l = 32, int in_max_adapt_delta_l = 32) : in_file(fopen(filename, "rb")), frames_left(0), atoms(0), istart(0), nsavc(1), delta(1.0), octree_index_decoder(in_file, &bit_buffer), array_index_decoder(in_file, &bit_buffer) {};
        
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
        unsigned int read_uint32();
        
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
        
        /* Data from the DCD file header */
        int istart;
        int nsavc;
        double delta;
        
        /* Decoders */
        OmelDecoder octree_index_decoder;
        OmelDecoder array_index_decoder;
};