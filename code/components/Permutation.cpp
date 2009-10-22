#include "Permutation.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string>

std::string get_permutation_env()
{
    char * perm = getenv("PERMUTATION");
    if (!perm)
        return "best";
    else return perm;
}

const char * perm_error_msg = "ERROR: PERMUTATION environment variable must "
    "be 'null', 'naive', 'delta', 'interframe' or 'best' (default)\n";

PermutationWriter * PermutationWriter::get_writer(ArithmeticEncoder * enc,
                                                  int size)
{
    std::string perm = get_permutation_env();
    if (perm == "null")
        return new NullPermutationWriter();
    if (perm == "naive")
        return new NaivePermutationWriter(enc);
    if (perm == "delta")
        return new DeltaPermutationWriter(enc);
    if (perm == "interframe")
        return new InterframePermutationWriter(enc, size);
    if (perm == "best")
        return new BestPermutationWriter(enc, size);
    fprintf(stderr, "%s", perm_error_msg);
    exit(1);
}


PermutationReader * PermutationReader::get_reader(ArithmeticDecoder * dec,
                                                  int size)
{
    std::string perm = get_permutation_env();
    if (perm == "null")
        return new NullPermutationReader();
    if (perm == "naive")
        return new NaivePermutationReader(dec);
    if (perm == "delta")
        return new DeltaPermutationReader(dec);
    if (perm == "interframe")
        return new InterframePermutationReader(dec, size);
    if (perm == "best")
        return new BestPermutationReader(dec, size);
    fprintf(stderr, "%s", perm_error_msg);
    exit(1);
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
    : m_size(size), m_size_orig(size)
{
    reset();
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


void IndexToSymbol::reset()
{
    m_size = m_size_orig;
    m_indicies.resize(m_size);
    m_symbols.resize(m_size);
    for (int i = 0; i < m_size; i++)
        m_indicies[i] = m_symbols[i] = i;
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


//
// InterframePermutation
//

InterframePermutationWriter::InterframePermutationWriter(ArithmeticEncoder * enc, int size)
    : m_enc(enc), m_pos(0)
{
    m_last.resize(size);
    for (int i = 0; i < size; i++)
        m_last[i] = i;
}

void InterframePermutationWriter::next_index(int index)
{
    int delta     = index - m_last[m_pos];
    m_last[m_pos] = index;
    m_enc.encode_int(delta);
    m_pos++;
}


InterframePermutationReader::InterframePermutationReader(ArithmeticDecoder * dec, int size)
    : m_dec(dec), m_pos(0)
{
    m_last.resize(size);
    for (int i = 0; i < size; i++)
        m_last[i] = i;
}


int InterframePermutationReader::next_index()
{
    int delta     = m_dec.decode_int();
    int index     = delta + m_last[m_pos];
    m_last[m_pos] = index;
    m_pos++;
    return index;
}
