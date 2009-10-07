#pragma once

#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/ByteEncoder.h"
#include "arithmetic/ByteDecoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

#include <set>


// Experiments with permutation compressors

class PermutationWriter
{
public:
    virtual ~PermutationWriter() {}
    virtual void next_index(int index) = 0;
};

class PermutationReader
{
public:
    virtual ~PermutationReader() {}
    virtual int next_index() = 0;
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

class BestPermutationWriter : public PermutationWriter
{
public:
    BestPermutationWriter(ArithmeticEncoder * enc, int size);
    void next_index(int index);
private:
    ArithmeticEncoder * m_enc;
    std::set<int> m_indicies;
};

class BestPermutationReader : public PermutationReader
{
public:
    BestPermutationReader(ArithmeticDecoder * dec, int size);
    int next_index();
private:
    ArithmeticDecoder * m_dec;
    std::set<int> m_indicies;
};
