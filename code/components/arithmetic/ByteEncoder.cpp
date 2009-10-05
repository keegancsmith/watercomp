#include "ByteEncoder.h"

ByteEncoder::ByteEncoder(ArithmeticEncoder* enc)
    : m_encoder(enc)
{
}


void ByteEncoder::encode(const void *ptr, size_t size, size_t nmemb)
{
    size_t total = size * nmemb;
    unsigned char *c = (unsigned char*)ptr;
    for(size_t i = 0; i < total; i++) {
        unsigned long long symbol = c[i];
        m_encoder->encode(symbol, symbol+1, 256);
    }
}
