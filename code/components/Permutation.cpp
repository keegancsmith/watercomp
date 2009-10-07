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

IndexToSymbol::IndexToSymbol(int size)
    : m_size(size)
{
    m_indicies.resize(size);
    m_symbols.resize(size);
    for (int i = 0; i < size; i++)
        m_indicies[i] = m_symbols[i] = i;
}


int IndexToSymbol::pop_index(int index)
{
    assert(m_size != 0);
    m_size--;

    int s = m_symbols[index];
    int l = m_indicies[m_size];

    m_symbols[l] = s;
    m_indicies[s] = l;

    return s;
}


int IndexToSymbol::pop_symbol(int symbol)
{
    int i = m_indicies[symbol];
    pop_index(i);
    return i;
}


int IndexToSymbol::size() const
{
    return m_size;
}


BestPermutationWriter::BestPermutationWriter(ArithmeticEncoder * enc,
                                             int size)
    : m_enc(enc), m_indicies(size)
{
}


void BestPermutationWriter::next_index(int index)
{
    int size = m_indicies.size();
    int val  = m_indicies.pop_index(index);
    m_enc->encode(val, val+1, size);
}


BestPermutationReader::BestPermutationReader(ArithmeticDecoder * dec,
                                             int size)
    : m_dec(dec), m_indicies(size)
{
}


int BestPermutationReader::next_index()
{
    int val = m_dec->decode(m_indicies.size());
    m_dec->decoder_update(val, val+1);
    return m_indicies.pop_symbol(val);
}
