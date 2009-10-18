#include "WaterReader.h"

#include "arithmetic/AdaptiveModelDecoder.h"
#include "arithmetic/ByteDecoder.h"
#include "splitter/FrameSplitter.h"

#include <cassert>

using std::vector;

WaterReader::WaterReader(FILE * fin, const vector<AtomInformation> & pdb)
    : m_fin(fin)
{
    split_frame(pdb, m_water_molecules, m_other_atoms);
}


void WaterReader::start()
{
    m_decoder.start_decode(m_fin);
    ByteDecoder dec(&m_decoder);

    // Read file header
    int header_int[2];
    dec.decode(header_int, sizeof(int), 2);
    m_natoms   = header_int[0];
    m_nframes  = header_int[1];
    //int ISTART = header_int[2];
    //int NSAVC  = header_int[3];
    
    //double DELTA;
    //dec.decode(&DELTA, sizeof(double), 1);

    // TODO atm we are ignoring most of these values!
}


bool WaterReader::next_frame(QuantisedFrame & qframe)
{
    assert(qframe.natoms() == natoms());

    next_frame_header(qframe);
    next_frame_water(qframe);
    next_frame_other(qframe);

    return true;
}


void WaterReader::end()
{
}


void WaterReader::next_frame_header(QuantisedFrame & qframe)
{
    ByteDecoder dec(&m_decoder);

    unsigned int header_quant[3];
    dec.decode(header_quant, sizeof(int), 3);
    dec.decode(qframe.min_coord, sizeof(float), 3);
    dec.decode(qframe.max_coord, sizeof(float), 3);

    qframe.m_xquant = header_quant[0];
    qframe.m_yquant = header_quant[1];
    qframe.m_zquant = header_quant[2];
}


void WaterReader::next_frame_water(QuantisedFrame & qframe)
{
    AdaptiveModelDecoder dec(&m_decoder);

    for (size_t i = 0; i < m_water_molecules.size(); i++) {
        WaterMolecule mol = m_water_molecules[i];
        unsigned int idx[3] = { mol.OH2_index, mol.H1_index, mol.H2_index };
        for (int j = 0; j < 3; j++)
            for (int d = 0; d < 3; d++)
                qframe.quantised_frame[3*idx[j] + d] = dec.decode_int();
    }
}


void WaterReader::next_frame_other(QuantisedFrame & qframe)
{
    AdaptiveModelDecoder dec(&m_decoder);

    for (size_t i = 0; i < m_other_atoms.size(); i++) {
        int idx = m_other_atoms[i];
        for (int d = 0; d < 3; d++)
            qframe.quantised_frame[3*idx + d] = dec.decode_int();
    }
}
