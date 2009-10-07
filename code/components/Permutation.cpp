#include "Permutation.h"

#include <string>
#include <cassert>
#include <cstdlib>

void encode_int(AdaptiveModelEncoder & enc, int i)
{
    char buf[12]; // Ints can only be 10 digits + sign + null
    sprintf(buf, "%d", i);
    enc.encode(buf);
}


int decode_int(AdaptiveModelDecoder & dec)
{
    std::string val = dec.decode();
    assert(val.size() < 12);
    return atoi(val.c_str());
}

//
// NaivePermutation
//

NaivePermutationWriter::NaivePermutationWriter(ArithmeticEncoder * enc)
    : m_enc(enc)
{
}


void NaivePermutationWriter::next_index(int index)
{
    m_enc.encode(&index, sizeof(index), 1);
}


NaivePermutationReader::NaivePermutationReader(ArithmeticDecoder * dec)
    : m_dec(dec)
{
}


int NaivePermutationReader::next_index()
{
    int index;
    m_dec.decode(&index, sizeof(index), 1);
    return index;
}

//
// DeltaPermutation
//

DeltaPermutationWriter::DeltaPermutationWriter(ArithmeticEncoder * enc)
    : m_enc(enc), m_last(0)
{
}


void DeltaPermutationWriter::next_index(int index)
{
    int delta = index - m_last;
    m_last    = index;
    encode_int(m_enc, delta);
}


DeltaPermutationReader::DeltaPermutationReader(ArithmeticDecoder * dec)
    : m_dec(dec), m_last(0)
{
}


int DeltaPermutationReader::next_index()
{
    int delta = decode_int(m_dec);
    int index = delta + m_last;
    m_last    = index;
    return index;
}
