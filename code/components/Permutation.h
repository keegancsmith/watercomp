#pragma once

#ifdef ANN
#include <ANN/ANN.h>
#endif

#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/ByteEncoder.h"
#include "arithmetic/ByteDecoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"
#include "pdbio/AtomInformation.h"
#include "Compressor.h"

#include <vector>
#include <string>
#include <map>

// Experiments with permutation compressors

extern std::string permutation_compressors[];

class PermutationWriter
{
public:
    virtual ~PermutationWriter() {}
    virtual void next_index(int index, unsigned int coord[3] = 0) = 0;
    virtual void reset() {}

    static PermutationWriter * get_writer(Compressor * compressor,
                                          ArithmeticEncoder * enc,
                                          int size);
};

class PermutationReader
{
public:
    virtual ~PermutationReader() {}
    virtual int next_index(unsigned int coord[3] = 0) = 0;
    virtual void reset() {}

    static PermutationReader * get_reader(Compressor * compressor,
                                          ArithmeticDecoder * dec,
                                          int size);
};


// NullPermutation has the effect of not storing the permutation
class NullPermutationWriter : public PermutationWriter
{
public:
    void next_index(int index, unsigned int coord[3] = 0) {}
};

class NullPermutationReader : public PermutationReader
{
public:
    NullPermutationReader() : m_index(0) {}
    int next_index(unsigned int coord[3] = 0) { return m_index++; }
    void reset() { m_index = 0; }
private:
    int m_index;
};


// NaivePermutation Writes out the index information as 32-bit ints
class NaivePermutationWriter : public PermutationWriter
{
public:
    NaivePermutationWriter(ArithmeticEncoder * enc);
    void next_index(int index, unsigned int coord[3] = 0);
private:
    ByteEncoder m_enc;
};

class NaivePermutationReader : public PermutationReader
{
public:
    NaivePermutationReader(ArithmeticDecoder * dec);
    int next_index(unsigned int coord[3] = 0);
private:
    ByteDecoder m_dec;
};


// DeltaPermutation encodes each index as the previous index - index
class DeltaPermutationWriter : public PermutationWriter
{
public:
    DeltaPermutationWriter(ArithmeticEncoder * enc);
    void next_index(int index, unsigned int coord[3] = 0);
    void reset() { m_last = 0; }
private:
    AdaptiveModelEncoder m_enc;
    int m_last;
};

class DeltaPermutationReader : public PermutationReader
{
public:
    DeltaPermutationReader(ArithmeticDecoder * dec);
    int next_index(unsigned int coord[3] = 0);
    void reset() { m_last = 0; }
private:
    AdaptiveModelDecoder m_dec;
    int m_last;
};


// OptimalPermutation encodes values using an arithmetic coder where as each
// value is encoded, it's symbol is removed from the arithmetic coder
// frequency table
class IndexToSymbol
{
public:
    IndexToSymbol(int size);
    int pop_index(int index);
    int pop_symbol(int symbol);
    int size() const;
    void reset();
private:
    int m_size, m_size_orig;
    std::vector<int> m_indicies;
    std::vector<int> m_symbols;
};

class OptimalPermutationWriter : public PermutationWriter
{
public:
    OptimalPermutationWriter(ArithmeticEncoder * enc, int size);
    void next_index(int index, unsigned int coord[3] = 0);
    void reset() { m_indicies.reset(); }
private:
    ArithmeticEncoder * m_enc;
    IndexToSymbol m_indicies;
};

class OptimalPermutationReader : public PermutationReader
{
public:
    OptimalPermutationReader(ArithmeticDecoder * dec, int size);
    int next_index(unsigned int coord[3] = 0);
    void reset() { m_indicies.reset(); }
private:
    ArithmeticDecoder * m_dec;
    IndexToSymbol m_indicies;
};


// InterframePermutation encodes each index as the delta of the previous
// index.
class InterframePermutationWriter : public PermutationWriter
{
public:
    InterframePermutationWriter(ArithmeticEncoder * enc, int size);
    void next_index(int index, unsigned int coord[3] = 0);
    void reset() { m_pos = 0; }
private:
    std::vector<int> m_last;
    AdaptiveModelEncoder m_enc;
    int m_pos;
};

class InterframePermutationReader : public PermutationReader
{
public:
    InterframePermutationReader(ArithmeticDecoder * dec, int size);
    int next_index(unsigned int coord[3] = 0);
    void reset() { m_pos = 0; }
private:
    std::vector<int> m_last;
    AdaptiveModelDecoder m_dec;
    int m_pos;
};


// PDBPermutation encodes each index using the information from the PDB file
class PDBPermutationWriter : public PermutationWriter
{
public:
    PDBPermutationWriter(ArithmeticEncoder * enc,
                         const std::vector<AtomInformation> & atom_info);
    void next_index(int index, unsigned int coord[3] = 0);
private:
    AdaptiveModelEncoder m_atom_name;
    AdaptiveModelEncoder m_residue_name;
    AdaptiveModelEncoder m_residue_sequence;
    AdaptiveModelEncoder m_seg_id;
    const std::vector<AtomInformation> & m_atom_info;
};

class PDBPermutationReader : public PermutationReader
{
public:
    PDBPermutationReader(ArithmeticDecoder * dec,
                         const std::vector<AtomInformation> & atom_info);
    int next_index(unsigned int coord[3] = 0);
private:
    struct pdb_key {
        std::string atom_name;
        std::string residue_name;
        unsigned int residue_sequence;
        std::string seg_id;

        bool operator < (const pdb_key & a) const;
    };

    std::map<pdb_key, int> m_pdb_to_index;

    AdaptiveModelDecoder m_atom_name;
    AdaptiveModelDecoder m_residue_name;
    AdaptiveModelDecoder m_residue_sequence;
    AdaptiveModelDecoder m_seg_id;
};


#ifdef ANN

// NearestNeighbourPermutation encodes each index as the rank distance from
// its position in the previous frame.
class NearestNeighbourPermutationWriter : public PermutationWriter
{
public:
    NearestNeighbourPermutationWriter(ArithmeticEncoder * enc, int size);
    void next_index(int index, unsigned int coord[3] = 0);
    void reset();
private:
    ANNpointArray m_current_points;
    ANNpointArray m_last_points;
    ANNkd_tree *m_last_tree;
    ANNidxArray m_nn_idx;
    ANNdistArray m_dd;

    int m_size;
    AdaptiveModelEncoder m_enc;
};

class NearestNeighbourPermutationReader : public PermutationReader
{
public:
    NearestNeighbourPermutationReader(ArithmeticDecoder * dec, int size);
    int next_index(unsigned int coord[3] = 0);
    void reset();
private:
    ANNpointArray m_current_points;
    ANNpointArray m_last_points;
    ANNkd_tree *m_last_tree;
    ANNidxArray m_nn_idx;
    ANNdistArray m_dd;

    int m_size;
    AdaptiveModelDecoder m_dec;
};

#endif
