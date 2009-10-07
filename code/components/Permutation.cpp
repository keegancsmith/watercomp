#include "Permutation.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string>


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
    m_enc.encode_int(delta);
}


DeltaPermutationReader::DeltaPermutationReader(ArithmeticDecoder * dec)
    : m_dec(dec), m_last(0)
{
}


int DeltaPermutationReader::next_index()
{
    int delta = m_dec.decode_int();
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
    for (int i = 0; i < val; i++)
        ++it;

    assert(it != m_indicies.end());
    int index = *it;
    m_indicies.erase(it);

    return index;
}
