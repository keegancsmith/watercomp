#pragma once

#include <cstdio>

class ArithmeticEncoder
{
    public:
        
        FILE* output_file;
        
        /// Only use 59 bits of the long long, giving a range of 0 to 2^59 - 1
        unsigned long long encoder_low;
        unsigned long long encoder_high;
        
        unsigned int e3_scaling;
        
        static const unsigned long long first_quarter = 0x200000000000000ULL;
        static const unsigned long long half_range = 0x400000000000000ULL;
        static const unsigned long long third_quarter = 0x600000000000000ULL;
        
        unsigned int bit_pool;
        unsigned int bits_left;
                
        void inline put_bit(bool bit)
        {
            bit_pool =  (bit_pool << 1) | bit;
            --bits_left;
            
            if(bits_left == 0)
            {
                bits_left = 8;
                fputc(bit_pool, output_file);
                bit_pool = 0;
            }
        }
        
        ArithmeticEncoder();
        
        void inline start_encode(FILE* file)
        {
            output_file = file; 
        }
        
        void inline encode(unsigned long long symbol_range_start, unsigned long long symbol_range_end, unsigned long long total)
        {
            unsigned long long step_size = (encoder_high - encoder_low + 1)/total;
    
            encoder_high = encoder_low + step_size*symbol_range_end - 1;
            encoder_low = encoder_low + step_size*symbol_range_start;
            
            while(true)
            {
                if(encoder_high < half_range)
                {
                    put_bit(0);
                    encoder_low <<= 1ULL;
                    encoder_high = 1 + (encoder_high<<1ULL);
                    
                    while(e3_scaling)
                    {
                        put_bit(1);
                        --e3_scaling;
                    }
                }
                else if(encoder_low >= half_range)
                {
                    put_bit(1);
                    encoder_low = (encoder_low - half_range)<<1ULL;
                    encoder_high = 1 + ((encoder_high - half_range)<<1ULL);
                    
                    while(e3_scaling)
                    {
                        put_bit(0);
                        --e3_scaling;
                    }
                }
                else
                    break;
            }
            
            while(encoder_low >= first_quarter && encoder_high < third_quarter)
            {
                ++e3_scaling;
                
                encoder_low = (encoder_low - first_quarter)<<1ULL;
                encoder_high = 1 + ((encoder_high - first_quarter)<<1ULL);
            }
        }
        
        void inline end_encode()
        {
            if(encoder_low < first_quarter)
            {
                put_bit(0);
                put_bit(1);
                
                while(e3_scaling)
                {
                    put_bit(1);
                    --e3_scaling;
                }
            }
            else
            {
                put_bit(1);
                put_bit(0);
                
                while(e3_scaling)
                {
                    put_bit(0);
                    --e3_scaling;
                }
            }
            
            while(bits_left != 8)
                put_bit(0);
        }
};