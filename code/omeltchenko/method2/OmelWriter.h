#pragma once
#include <cstdio>
#include <queue>
#include <vector>
#include "Atom.h"
#include "OmelEncoder.h"

class OmelWriter
{
    public:
        /* Constructs a Omeltchenko reader object to read from the specified file. */
        OmelWriter(const char* filename, int in_initial_l = 3, int in_delta_l = 2, int in_max_adapt_initial_l = 32, int in_max_adapt_delta_l = 32) : out_file(fopen(filename, "wb")), frames_written(0), atoms(0), sort_num_buckets(0), sort_buckets(NULL), octree_index_encoder(OmelEncoder(out_file, &bit_buffer)), array_index_encoder(OmelEncoder(out_file, &bit_buffer)) {};
        
        /* Destructor - deletes the bucket array */
        ~OmelWriter();
        
        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start_write(int ISTART = 0, int NSAVC = 1, double DELTA = 1.0);
        
        /* Flushes the bit queue*/
        void flush();
        
        /* Ends the reading process. */
        void end_write();
        
        /* Writes a frame */
        bool write_frame(std::vector<Atom>& atom_list, int x_step_bits, int y_step_bits, int z_step_bits);
        
        /* Accessor for number of atoms */
        int get_atoms() { return atoms; };
        
        /* Accessor for frames written */
        int get_frames() { return frames_written; };
        
    private:
        /* File for saving */
        FILE* out_file;
        
        /* Writes a bit to the stream */
        bool put_bit(bool bit);
        
        /* Writes a 32-bit uncompressed unsigned int */
        void write_uint32(unsigned int num);
        
        /* UNUSED - This is how they describe the sorting process, but in most cases STL is faster */
        void sort_indices(std::vector<unsigned int>& indices);
        
        /* A bit buffer for storing bits read, but not used */
        std::queue<bool> bit_buffer;
        
        /* Number of frames */
        int frames_written;
        
        /* Number of atoms in each frame */
        int atoms;
        
        /* Bucket sorting */
        unsigned int sort_num_buckets;
        std::vector<unsigned int>* sort_buckets;
        
        /* Encoders */
        OmelEncoder octree_index_encoder;
        OmelEncoder array_index_encoder;
        
        /* order[i] = Index in file, starts off as order[i] = i */
        std::vector<unsigned int> order;
};