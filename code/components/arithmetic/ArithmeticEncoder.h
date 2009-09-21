#pragma once

#include <cstdio>
#include <queue>

class ArithmeticEncoder
{
    public:
        
        FILE* output_file;
        
        /// Only use 59 bits of the long long, giving a range of 0 to 2^59 - 1
        unsigned long long encoder_low;
        unsigned long long encoder_high;
        
        unsigned int e3_scaling;
        
        static const unsigned long long first_quarter = 0x200000000000000LL;
        static const unsigned long long half_range = 0x400000000000000LL;
        static const unsigned long long third_quarter = 0x600000000000000LL;
        
        void flush();
        void put_bit(bool bit);
        
        std::queue<bool> bit_buffer;
        
        ArithmeticEncoder();
        
        void start_encode(FILE* file);
        
        void encode(unsigned long long symbol_range_start, unsigned long long symbol_range_end, unsigned long long total);
        
        void end_encode();
};