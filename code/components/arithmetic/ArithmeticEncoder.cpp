#include "ArithmeticEncoder.h"

using namespace std;

ArithmeticEncoder::ArithmeticEncoder()
{
    encoder_low = 0;
    encoder_high = 0x7FFFFFFFFFFFFFFLL;
    e3_scaling = 0;
    
    bit_pool = 0;
    bits_left = 8;
}

// void inline ArithmeticEncoder::start_encode(FILE* file)
// {
// }
// 
// void inline ArithmeticEncoder::encode(unsigned long long symbol_range_start, unsigned long long symbol_range_end, unsigned long long total)
// {    
//     
// }
// 
// void inline ArithmeticEncoder::end_encode()
// {
// }
// 
// void inline ArithmeticEncoder::put_bit(bool bit)
// {
// }