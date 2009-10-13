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