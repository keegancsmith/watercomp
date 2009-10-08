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
