#pragma once
#include "ArithmeticEncoder.h"

// Class to write raw bytes to Arithmetic Encoder
class ByteEncoder
{
public:
    ByteEncoder(ArithmeticEncoder* enc);
    void encode(const void *ptr, size_t size, size_t nmemb);
private:
    ArithmeticEncoder* m_encoder;
};
