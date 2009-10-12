#pragma once

#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/ByteEncoder.h"
#include "arithmetic/ByteDecoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

#include <vector>


// Experiments with permutation compressors

class PermutationWriter
{
public:
    virtual ~PermutationWriter() {}
    virtual void next_index(int index) = 0;

    static PermutationWriter * get_writer(ArithmeticEncoder * enc,
                                          int size);
};

class PermutationReader
{
public:
    virtual ~PermutationReader() {}
    virtual int next_index() = 0;

    static PermutationReader * get_reader(ArithmeticDecoder * dec,
                                          int size);
};


// NullPermutation has the effect of not storing the permutation
class NullPermutationWriter : public PermutationWriter
{
public:
    void next_index(int index) {}
};

class NullPermutationReader : public PermutationReader
{
public:
    NullPermutationReader() : m_index(0) {}
    int next_index() { return m_index++; }
private:
    int m_index;
};


// NaivePermutation Writes out the index information as 32-bit ints
class NaivePermutationWriter : public PermutationWriter
{
public:
    NaivePermutationWriter(ArithmeticEncoder * enc);
    void next_index(int index);
private:
    ByteEncoder m_enc;
};

class NaivePermutationReader : public PermutationReader
{
public:
    NaivePermutationReader(ArithmeticDecoder * dec);
    int next_index();
private:
    ByteDecoder m_dec;
};


// DeltaPermutation encodes each index as the previous index - index
class DeltaPermutationWriter : public PermutationWriter
{
public:
    DeltaPermutationWriter(ArithmeticEncoder * enc);
    void next_index(int index);
private:
    AdaptiveModelEncoder m_enc;
    int m_last;
};

class DeltaPermutationReader : public PermutationReader
{
public:
    DeltaPermutationReader(ArithmeticDecoder * dec);
    int next_index();
private:
    AdaptiveModelDecoder m_dec;
    int m_last;
};


// BestPermutation encodes values using an arithmetic coder where as each
// value is encoded, it's symbol is removed from the arithmetic coder
// frequency table
class IndexToSymbol
{
public:
    IndexToSymbol(int size);
    int pop_index(int index);
    int pop_symbol(int symbol);
    int size() const;
private:
    int m_size;
    std::vector<int> m_indicies;
    std::vector<int> m_symbols;
};

class BestPermutationWriter : public PermutationWriter
{
public:
    BestPermutationWriter(ArithmeticEncoder * enc, int size);
    void next_index(int index);
private:
    ArithmeticEncoder * m_enc;
    IndexToSymbol m_indicies;
};

class BestPermutationReader : public PermutationReader
{
public:
    BestPermutationReader(ArithmeticDecoder * dec, int size);
    int next_index();
private:
    ArithmeticDecoder * m_dec;
    IndexToSymbol m_indicies;
};


// InterframePermutation encodes each index as the delta of the previous
// index. Depends on a global variable, so not to safe to use more than once!
class InterframePermutationWriter : public PermutationWriter
{
public:
    InterframePermutationWriter(ArithmeticEncoder * enc, int size);
    void next_index(int index);
    static void reset_last(int size) {
        m_last.resize(size);
        for (int i = 0; i < size; i++)
            m_last[i] = i;
    }
private:
    static std::vector<int> m_last;
    AdaptiveModelEncoder m_enc;
    int m_pos;
};

class InterframePermutationReader : public PermutationReader
{
public:
    InterframePermutationReader(ArithmeticDecoder * dec, int size);
    int next_index();
    static void reset_last(int size) {
        m_last.resize(size);
        for (int i = 0; i < size; i++)
            m_last[i] = i;
    }
private:
    static std::vector<int> m_last;
    AdaptiveModelDecoder m_dec;
    int m_pos;
};
