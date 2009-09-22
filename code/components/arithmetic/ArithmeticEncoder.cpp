#include "ArithmeticEncoder.h"

using namespace std;

ArithmeticEncoder::ArithmeticEncoder()
{
    encoder_low = 0;
    encoder_high = 0x7FFFFFFFFFFFFFFLL;
    e3_scaling = 0;
}

void ArithmeticEncoder::start_encode(FILE* file)
{
    output_file = file;
}

void ArithmeticEncoder::encode(unsigned long long symbol_range_start, unsigned long long symbol_range_end, unsigned long long total)
{    
    unsigned long long step_size = (encoder_high - encoder_low + 1)/total;
    
    encoder_high = encoder_low + step_size*symbol_range_end - 1;
    encoder_low = encoder_low + step_size*symbol_range_start;
    
    while(true)
    {
        if(encoder_high < half_range)
        {
            put_bit(0);
            encoder_low *= 2;
            encoder_high = 2*encoder_high + 1;
            
            while(e3_scaling)
            {
                put_bit(1);
                --e3_scaling;
            }
        }
        else if(encoder_low >= half_range)
        {
            put_bit(1);
            encoder_low = 2*(encoder_low - half_range);
            encoder_high = 2*(encoder_high - half_range) + 1;
            
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
        
        encoder_low = 2*(encoder_low - first_quarter);
        encoder_high = 2*(encoder_high - first_quarter) + 1;
    }
}

void ArithmeticEncoder::end_encode()
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
    
    while(bit_buffer.size() % 8 != 0)
        put_bit(0);
    
    flush();
}

void ArithmeticEncoder::flush()
{
    while(bit_buffer.size() >= 8)
    {
        unsigned char acc = 0;
        for(int i = 0; i < 8; ++i)
        {
            acc = (acc << 1) | bit_buffer.front();
            bit_buffer.pop();
        }
        fprintf(output_file, "%c", acc);
    }
}

void ArithmeticEncoder::put_bit(bool bit)
{
    bit_buffer.push(bit);
    
    if(bit_buffer.size() >= 1024)
        flush();
}