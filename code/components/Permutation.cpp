#include "Permutation.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string>


//
// Utilities to encode/decode ints to Arithmetic Coders
//

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


//
// BestPermutation
//

// TODO use a better data structure (Fenwick Tree. For decoding you can do a
// binary search over the cumulative sums to find the corresponding index)
BestPermutationWriter::BestPermutationWriter(ArithmeticEncoder * enc,
                                             int size)
    : m_enc(enc)
{
    for (int i = 0; i < size; i++)
        m_indicies.insert(i);
}


void BestPermutationWriter::next_index(int index)
{
    std::set<int>::iterator it = m_indicies.find(index);
    assert(it != m_indicies.end());

    int size = m_indicies.size();
    int val  = std::distance(m_indicies.begin(), it);

    m_enc->encode(val, val+1, size);

    m_indicies.erase(it);
}


BestPermutationReader::BestPermutationReader(ArithmeticDecoder * dec,
                                             int size)
    : m_dec(dec)
{
    for (int i = 0; i < size; i++)
        m_indicies.insert(i);
}


int BestPermutationReader::next_index()
{
    int val = m_dec->decode(m_indicies.size());
    m_dec->decoder_update(val, val+1);

    std::set<int>::iterator it = m_indicies.begin();
    int index = 0;
    while(*it != index) {
        ++index;
        ++it;
        assert(it != m_indicies.end());
    }

    m_indicies.erase(it);

    return index;
}
