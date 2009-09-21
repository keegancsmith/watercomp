#include "ArithmeticDecoder.h"
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

ArithmeticDecoder::ArithmeticDecoder()
{
    decoder_low = 0;
    decoder_high = 0x7FFFFFFFFFFFFFFLL;
    e3_scaling = 0;
    
    decoder_buffer = 0;
    decoder_step = -1;
}

void ArithmeticDecoder::start_decode(FILE* file)
{
    input_file = file;
    
    for(int i = 0; i < 59; i++)
        decoder_buffer = (decoder_buffer << 1) | get_bit();
}

unsigned long long ArithmeticDecoder::decode(unsigned long long total)
{
    decoder_step = (decoder_high - decoder_low + 1)/total;
    unsigned long long ret_val = (decoder_buffer - decoder_low)/decoder_step;
    
    return ret_val;
}

void ArithmeticDecoder::decoder_update(unsigned long long symbol_range_start, unsigned long long symbol_range_end)
{
    decoder_high = decoder_low + decoder_step*symbol_range_end - 1;
    decoder_low = decoder_low + decoder_step*symbol_range_start;
    
    while(true)
    {
        if(decoder_high < half_range)
        {
            decoder_low *= 2;
            decoder_high = 2*decoder_high + 1;
            decoder_buffer = 2*decoder_buffer + get_bit();
        }
        else if(decoder_low >= half_range)
        {
            decoder_low = 2*(decoder_low - half_range);
            decoder_high = 2*(decoder_high - half_range) + 1;
            decoder_buffer = 2*(decoder_buffer - half_range) + get_bit();
        }
        else
            break;
    }
    
    while(decoder_low >= first_quarter && decoder_high < third_quarter)
    {
        decoder_low = 2*(decoder_low - first_quarter);
        decoder_high = 2*(decoder_high - first_quarter) + 1;
        decoder_buffer = 2*(decoder_buffer - first_quarter) + get_bit();
    }
}

// unsigned int ArithmeticDecoder::decode()
// {
//     if(is_eof)
//         return 2;
//     
// //     printf("%llX %llX %lld %llX\n", decoder_high, decoder_low, frequencies.query(frequencies.size), decoder_buffer);
// //     decoder_step = (decoder_high - decoder_low + 1)/frequencies.query(frequencies.size);
// //     long long ret_val = (decoder_buffer - decoder_low)/decoder_step;
// //     printf("ANS = %llX\n", ret_val);
// 
// //     printf("%d %lld\n", frequencies.size-1, frequencies.query(frequencies.size-1));
//     unsigned long long frequency = decode(frequencies.total_frequency);
//     unsigned int symbol = frequencies.cumulative_frequency(frequency);
// //     printf("%lld is %u\n", frequency, symbol);
//     decoder_update(frequencies.query(symbol - 1), frequencies.query(symbol));
//     
// //     printf("CSB %lld %d\n", frequency, symbol);
//     
//     if(symbol == 1)
//     {
// //         frequencies.add_symbol();
// //         vector<char> new_symbol;
// //         unsigned int chr;
// //         
// //         do
// //         {
// //             chr = decode(256);
// // //             printf("I %c %d\n", chr, chr);
// //             decoder_update(chr, chr+1);
// //             
// //             new_symbol.push_back(chr);
// //         }
// //         while(chr != 0);
//         
//         frequencies.add_symbol();
// //         symbol_map[frequencies.add_symbol()] = string(new_symbol.begin(), new_symbol.end());
//         
// //         printf("New symbol: %s\n", string(new_symbol.begin(), new_symbol.end()).c_str());
//     }
//     else if(symbol == 2)
//     {
//         is_eof = true;
//         return 2;
//     }
//     
//     frequencies.update(symbol);
//     
//     return symbol;
// }

// void ArithmeticDecoder::end_decode()
// {
// }

bool ArithmeticDecoder::get_bit()
{
    if(bit_buffer.size() == 0)
    {
        unsigned char tmp;
        
        fread(&tmp, sizeof(unsigned char), 1, input_file);
        
        if(feof(input_file))
            tmp = 0;
        
        for(int i = 0; i < 8; ++i)
            bit_buffer.push(tmp&(1<<(7-i)));
    }
    
    bool ret_val = bit_buffer.front();
    bit_buffer.pop();
    
//     printf("%d", ret_val); 
    
    return ret_val;
}
