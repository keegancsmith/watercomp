#pragma once
#include <cstdio>
#include <queue>
#include <vector>
#include "Atom.h"

class OmelWriter
{
    public:
        /* Constructs a Omeltchenko reader object to read from the specified file. */
        OmelWriter(const char* filename, int in_initial_l = 3, int in_delta_l = 2, int in_max_adapt_initial_l = 32, int in_max_adapt_delta_l = 32);
        
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
        /* File required for input */
        FILE* out_file;
        
        /* Writes an unsigned 32-bit integer */
        void write_uncompressed_int(unsigned int num);
        
        /* Writes an compressed unsigned 32-bit integer */
        void write_compressed_int(unsigned int num);
        
        /* Writes a bit from the stream */
        bool put_bit(bool bit);
        
        /* Gets the number of bits required to represent the unsigned 32-bit integer */
        int num_bits(unsigned int num);
        
        /* UNUSED - This is how they describe the sorting process, but in most cases STL is faster */
        void sort_indices(std::vector<unsigned int>& indices);
        
        /* A bit buffer for storing bits read, but not used */
        std::queue<bool> bit_buffer;
        
        /* Number of frames */
        int frames_written;
        
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
        
        /* Bucket sorting */
        unsigned int sort_num_buckets;
        std::vector<unsigned int>* sort_buckets;
};