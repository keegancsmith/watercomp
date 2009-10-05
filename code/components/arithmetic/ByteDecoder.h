#pragma once
#include "ArithmeticDecoder.h"

// Inverse of ByteEncoder. Decodes byte streams
class ByteDecoder
{
public:
    ByteDecoder(ArithmeticDecoder * dec);
    void decode(void *ptr, size_t size, size_t nmemb);
private:
    ArithmeticDecoder* m_decoder;
};
