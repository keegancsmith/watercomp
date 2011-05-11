#include "Permutation.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string>

std::string permutation_compressors[] = {"optimal", "null", "naive", "delta",
                                         "interframe", "pdb",
#ifdef ANN
                                         "nearest",
#endif
                                         ""};

PermutationWriter * PermutationWriter::get_writer(Compressor * c,
                                                  ArithmeticEncoder * enc,
                                                  int size)
{
    std::string perm = c->m_permutation_compressor;
    if (perm == "null")
        return new NullPermutationWriter();
    if (perm == "naive")
        return new NaivePermutationWriter(enc);
    if (perm == "delta")
        return new DeltaPermutationWriter(enc);
    if (perm == "interframe")
        return new InterframePermutationWriter(enc, size);
    if (perm == "optimal")
        return new OptimalPermutationWriter(enc, size);
#ifdef ANN
    if (perm == "nearest")
        return new NearestNeighbourPermutationWriter(enc, size);
#endif
    if (perm == "pdb")
        return new PDBPermutationWriter(enc, c->m_atom_information);
    abort();
}


PermutationReader * PermutationReader::get_reader(Compressor * c,
                                                  ArithmeticDecoder * dec,
                                                  int size)
{
    std::string perm = c->m_permutation_compressor;
    if (perm == "null")
        return new NullPermutationReader();
    if (perm == "naive")
        return new NaivePermutationReader(dec);
    if (perm == "delta")
        return new DeltaPermutationReader(dec);
    if (perm == "interframe")
        return new InterframePermutationReader(dec, size);
    if (perm == "optimal")
        return new OptimalPermutationReader(dec, size);
#ifdef ANN
    if (perm == "nearest")
        return new NearestNeighbourPermutationReader(dec, size);
#endif
    if (perm == "pdb")
        return new PDBPermutationReader(dec, c->m_atom_information);
    abort();
}


//
// NaivePermutation
//

NaivePermutationWriter::NaivePermutationWriter(ArithmeticEncoder * enc)
    : m_enc(enc)
{
}


void NaivePermutationWriter::next_index(int index, unsigned int coord[3])
{
    m_enc.encode(&index, sizeof(index), 1);
}


NaivePermutationReader::NaivePermutationReader(ArithmeticDecoder * dec)
    : m_dec(dec)
{
}


int NaivePermutationReader::next_index(unsigned int coord[3])
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


void DeltaPermutationWriter::next_index(int index, unsigned int coord[3])
{
    int delta = index - m_last;
    m_last    = index;
    m_enc.encode_int(delta);
}


DeltaPermutationReader::DeltaPermutationReader(ArithmeticDecoder * dec)
    : m_dec(dec), m_last(0)
{
}


int DeltaPermutationReader::next_index(unsigned int coord[3])
{
    int delta = m_dec.decode_int();
    int index = delta + m_last;
    m_last    = index;
    return index;
}


//
// OptimalPermutation
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


OptimalPermutationWriter::OptimalPermutationWriter(ArithmeticEncoder * enc,
                                             int size)
    : m_enc(enc), m_indicies(size)
{
}


void OptimalPermutationWriter::next_index(int index, unsigned int coord[3])
{
    int size = m_indicies.size();
    int val  = m_indicies.pop_index(index);
    m_enc->encode(val, val+1, size);
}


OptimalPermutationReader::OptimalPermutationReader(ArithmeticDecoder * dec,
                                             int size)
    : m_dec(dec), m_indicies(size)
{
}


int OptimalPermutationReader::next_index(unsigned int coord[3])
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

void InterframePermutationWriter::next_index(int index, unsigned int coord[3])
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


int InterframePermutationReader::next_index(unsigned int coord[3])
{
    int delta     = m_dec.decode_int();
    int index     = delta + m_last[m_pos];
    m_last[m_pos] = index;
    m_pos++;
    return index;
}


//
// PDBPermutation
//

PDBPermutationWriter::PDBPermutationWriter(ArithmeticEncoder * enc,
                                           const std::vector<AtomInformation> & atom_info)
    : m_atom_name(enc), m_residue_name(enc), m_residue_sequence(enc),
      m_seg_id(enc), m_atom_info(atom_info)
{
}


void PDBPermutationWriter::next_index(int index, unsigned int coord[3])
{
    const AtomInformation & info = m_atom_info[index];
    m_atom_name.encode(info.atom_name);
    m_residue_name.encode(info.residue_name);
    m_residue_sequence.encode_int(info.residue_sequence);
    m_seg_id.encode(info.seg_id);
}


bool PDBPermutationReader::pdb_key::operator < (const pdb_key & a) const
{
    if (atom_name != a.atom_name)
        return atom_name < a.atom_name;
    if (residue_name != a.residue_name)
        return residue_name < a.residue_name;
    if (residue_sequence != a.residue_sequence)
        return residue_sequence < a.residue_sequence;
    return seg_id < a.seg_id;
}


PDBPermutationReader::PDBPermutationReader(ArithmeticDecoder * dec,
                                           const std::vector<AtomInformation> & atom_info)
  : m_atom_name(dec), m_residue_name(dec),
    m_residue_sequence(dec), m_seg_id(dec)
{
    for (size_t i = 0; i < atom_info.size(); i++) {
        const AtomInformation & info = atom_info[i];
        pdb_key k = { info.atom_name,
                      info.residue_name,
                      info.residue_sequence,
                      info.seg_id };
        m_pdb_to_index[k] = i;
    }
}


int PDBPermutationReader::next_index(unsigned int coord[3])
{
    using std::string;

    string atom_name = m_atom_name.decode();
    string residue_name = m_residue_name.decode();
    unsigned int residue_sequence = m_residue_sequence.decode_int();
    string seg_id = m_seg_id.decode();

    pdb_key k = { atom_name,
                  residue_name,
                  residue_sequence,
                  seg_id };

    return m_pdb_to_index[k];
}


#ifdef ANN

//
// NearestNeighbourPermutation
//

inline bool fequal(ANNcoord a, ANNcoord b) {
    return abs(a - b) < 1e-9;
}

struct SearchResultComparator {
    ANNpointArray m_points;
    ANNidxArray m_idx;
    ANNdistArray m_dist;

    bool operator() (int a, int b) const {
        if (!fequal(m_dist[a], m_dist[b]))
            return m_dist[a] < m_dist[b];

        int x = m_idx[a];
        int y = m_idx[b];
        for (int d = 0; d < 3; d++)
            if (!fequal(m_points[x][d], m_points[y][d]))
                return m_points[x][d] < m_points[y][d];

        return x < y;
    }
};



NearestNeighbourPermutationWriter::NearestNeighbourPermutationWriter(
    ArithmeticEncoder * enc, int size)
    : m_current_points(0), m_last_points(0), m_last_tree(0),
      m_size(size), m_enc(enc)
{
    m_nn_idx = new ANNidx[size];
    m_dd = new ANNdist[size];
}

void NearestNeighbourPermutationWriter::next_index(int index,
                                                   unsigned int coord[3])
{
    assert(coord != 0);

    ANNpoint point = annAllocPt(3);
    for (int d = 0; d < 3; d++)
        m_current_points[index][d] = point[d] = coord[d];

    if (!m_last_points) {
        // Just write out the index
        m_enc.encode_int(index);
        annDeallocPt(point);
        return;
    }

    // Otherwise we encode the distance rank the point is from the previous
    // frame.

    // Distance between the points between this frame and the last.
    ANNdist sqRad = 0;
    for (int d = 0; d < 3; d++) {
        ANNdist delta = coord[d] - m_last_points[index][d];
        sqRad += (delta * delta);
    }
    sqRad += 1e-9;

    // Get all points within sqrt(sqRad) of the current point
    int k = m_last_tree->annkFRSearch(point, sqRad, 0);
    m_last_tree->annkFRSearch(point, sqRad, k, m_nn_idx, m_dd);

    // We will have to use our own comparator to do tie breaks.
    SearchResultComparator comparator = { m_last_points, m_nn_idx, m_dd };
    int indicies[k];
    for (int i = 0; i < k; i++)
        indicies[i] = i;
    std::sort(indicies, indicies + k, comparator);

    while (m_nn_idx[indicies[--k]] != index)
        assert(k > 0);

    m_enc.encode_int(k);

    // Cleanup
    annDeallocPt(point);
}

void NearestNeighbourPermutationWriter::reset() {
    if (!m_current_points) { // First run
        m_current_points = annAllocPts(m_size, 3);
        return;
    }

    if (m_last_tree)
        delete m_last_tree;
    else
        m_last_points = annAllocPts(m_size, 3);

    std::swap(m_last_points, m_current_points);
    m_last_tree = new ANNkd_tree(m_last_points, m_size, 3);
}

NearestNeighbourPermutationReader::NearestNeighbourPermutationReader(
    ArithmeticDecoder * dec, int size)
    : m_current_points(0), m_last_points(0), m_last_tree(0),
      m_size(size), m_dec(dec)
{
    m_nn_idx = new ANNidx[size];
    m_dd = new ANNdist[size];
}

int NearestNeighbourPermutationReader::next_index(unsigned int coord[3])
{
    assert(coord != 0);

    if (!m_last_points) {
        // Just read in the index
        int index = m_dec.decode_int();
        for (int d = 0; d < 3; d++)
            m_current_points[index][d] = coord[d];
        return index;
    }

    // Otherwise we get the distance rank and decode from there.
    int rank = m_dec.decode_int();
    assert(0 <= rank && rank < m_size);

    ANNpoint point = annAllocPt(3);
    for (int d = 0; d < 3; d++)
        point[d] = coord[d];

    // Find out how far the kth point is
    m_last_tree->annkSearch(point, rank + 1, m_nn_idx, m_dd);
    ANNdist sqRad = 0;
    for (int i = 0; i <= rank; i++)
        sqRad = std::max(sqRad, m_dd[i]);
    sqRad += 1e-9;

    int k = m_last_tree->annkFRSearch(point, sqRad, 0);
    m_last_tree->annkFRSearch(point, sqRad, k, m_nn_idx, m_dd);
    assert(k >= rank);

    // We will have to use our own comparator to do tie breaks.
    SearchResultComparator comparator = { m_last_points, m_nn_idx, m_dd };
    int indicies[k];
    for (int i = 0; i < k; i++)
        indicies[i] = i;
    std::sort(indicies, indicies + k, comparator);

    int index = m_nn_idx[indicies[rank]];

    for (int d = 0; d < 3; d++)
        m_current_points[index][d] = coord[d];

    // Cleanup
    annDeallocPt(point);

    return index;
}

void NearestNeighbourPermutationReader::reset()
{
    if (!m_current_points) { // First run
        m_current_points = annAllocPts(m_size, 3);
        return;
    }

    if (m_last_tree)
        delete m_last_tree;
    else
        m_last_points = annAllocPts(m_size, 3);

    std::swap(m_last_points, m_current_points);
    m_last_tree = new ANNkd_tree(m_last_points, m_size, 3);
}


#endif // ANN
