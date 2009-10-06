#include "ByteDecoder.h"

ByteDecoder::ByteDecoder(ArithmeticDecoder * dec)
    : m_decoder(dec)
{
}


void ByteDecoder::decode(void *ptr, size_t size, size_t nmemb)
{
    size_t total = size * nmemb;
    unsigned char *c = (unsigned char*)ptr;
    for(size_t i = 0; i < total; i++)
        c[i] = m_decoder->decode(256);
}
