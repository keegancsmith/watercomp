#pragma once
#include <queue>
#include <cstdio>

class OmelDecoder
{
    public:
        OmelDecoder(FILE* out, std::queue<bool>* buffer, int in_initial_l = 3, int in_delta_l = 2, int in_max_adapt_initial_l = 64, int in_max_adapt_delta_l = 64);
        
        ~OmelDecoder();
        
        /* Reads a compressed unsigned 64-bit integer */
        unsigned long long read_uint64();
        
    private:
        
        /* File for saving data to. */
        FILE* in_file;
        
        /* Bit buffer that can be shared between multiple instances. */
        std::queue<bool>* bit_buffer;
        
        /* Records if the bit buffer was created by this object and should be deleted. */
        bool delete_buffer;
        
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
        
        /* Writes a bit to the stream */
        unsigned long long get_bit();
        
        /* Gets the number of bits required to represent the unsigned 32-bit integer */
        int num_bits(unsigned long long num);
};