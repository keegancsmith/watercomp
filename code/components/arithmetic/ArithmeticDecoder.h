#pragma once
#include <cstdio>
#include <queue>
#include <string>
#include <map>

class ArithmeticDecoder
{
    public:
        
        ArithmeticDecoder();
        
        void start_decode(FILE* file);
        void decoder_update(unsigned long long symbol_range_start, unsigned long long symbol_range_end);
        unsigned long long decode(unsigned long long total);
//         unsigned int decode();
//         void end_decode();
        
        bool get_bit();
        
        std::map<unsigned int, std::string> symbol_map;
        
        std::queue<bool> bit_buffer;
        
        FILE* input_file;
        
        bool is_eof;
        
        unsigned long long decoder_buffer;
        
        unsigned long long decoder_low;
        unsigned long long decoder_high;
        unsigned long long decoder_step;
        
        unsigned int e3_scaling;
        
        static const unsigned long long first_quarter = 0x200000000000000LL;
        static const unsigned long long half_range = 0x400000000000000LL;
        static const unsigned long long third_quarter = 0x600000000000000LL;
};