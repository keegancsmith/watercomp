#pragma once
#include <queue>
#include <cstdio>

class OmelEncoder
{
    public:
        /* 
         Constructs OmelEncoder stream.
         If buffer is NULL then the OmelEncoder constructs one.
         This should only be done if the OmelEncoder is the only
         thing using the file. Otherwise, shared buffer should
         be used
        */
        OmelEncoder(FILE* out, std::queue<bool>* buffer, int in_initial_l = 3, int in_delta_l = 2, int in_max_adapt_initial_l = 32, int in_max_adapt_delta_l = 32);
        
        /* Destructor */
        ~OmelEncoder();
        
        /* Writes an compressed unsigned 32-bit integer */
        void write_uint32(unsigned int num);
        
    private:
        /* File for saving data to. */
        FILE* out_file;
        
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
        bool put_bit(bool bit);
        
        /* Flushes the bit queue*/
        void flush();
        
        /* Gets the number of bits required to represent the unsigned 32-bit integer */
        int num_bits(unsigned int num);
};