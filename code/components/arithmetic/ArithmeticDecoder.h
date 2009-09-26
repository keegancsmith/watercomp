#pragma once
#include <cstdio>
#include <queue>
#include <string>
#include <map>
#include <vector>

class ArithmeticDecoder
{
    public:
        
        ArithmeticDecoder();
        
        void inline start_decode(FILE* file)
        {
            input_file = file;
    
            for(int i = 0; i < 59; ++i)
                decoder_buffer = (decoder_buffer << 1ULL) | get_bit();
        }
        
        void inline decoder_update(unsigned long long symbol_range_start, unsigned long long symbol_range_end)
        {
            decoder_high = decoder_low + decoder_step*symbol_range_end - 1;
            decoder_low = decoder_low + decoder_step*symbol_range_start;
            
            while(true)
            {
                if(decoder_high < half_range)
                {
                    decoder_low <<= 1ULL;
                    decoder_high = 1+(decoder_high << 1ULL);
                    decoder_buffer = get_bit() + (decoder_buffer << 1ULL);
                }
                else if(decoder_low >= half_range)
                {
                    decoder_low = (decoder_low - half_range) << 1ULL;
                    decoder_high = 1 + ((decoder_high - half_range) << 1ULL);
                    decoder_buffer = get_bit() + ((decoder_buffer - half_range) << 1ULL);
                }
                else
                    break;
            }
            
            while(decoder_low >= first_quarter && decoder_high < third_quarter)
            {
                decoder_low = (decoder_low - first_quarter) << 1ULL;
                decoder_high = 1 + ((decoder_high - first_quarter) << 1ULL);
                decoder_buffer = get_bit() + ((decoder_buffer - first_quarter) << 1ULL);
            }
        }
        
        unsigned long long inline decode(unsigned long long total)
        {
            decoder_step = (decoder_high - decoder_low + 1)/total;
            unsigned long long ret_val = (decoder_buffer - decoder_low)/decoder_step;
            
            return ret_val;
        }
        
        bool inline get_bit()
        {
            if(bits_left == 0)
            {
                bit_pool = getc(input_file);
                if(feof(input_file))
                    bit_pool = 0;
		
                bits_left = 8;
            }

            int output = (bit_pool & 0x80);
            bit_pool <<= 1;
            --bits_left;
            
            return output;
        }
        
        unsigned int bit_pool;
        unsigned int bits_left;
        
        FILE* input_file;
        
        unsigned long long decoder_buffer;
        
        unsigned long long decoder_low;
        unsigned long long decoder_high;
        unsigned long long decoder_step;
        
        unsigned int e3_scaling;
        
        static const unsigned long long first_quarter = 0x200000000000000LL;
        static const unsigned long long half_range = 0x400000000000000LL;
        static const unsigned long long third_quarter = 0x600000000000000LL;
};
