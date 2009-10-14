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
    
    bit_pool = 0;
    bits_left = 0;
}