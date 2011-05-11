#include "WaterReader.h"

#include "splitter/FrameSplitter.h"
#include "OxygenGraph.h"

#include <cassert>

using std::vector;

WaterReader::WaterReader(FILE * fin, Compressor * c)
    : m_adaptive(&m_decoder), m_byte(&m_decoder), m_compressor(c),
      m_previous_frame(0, 8, 8, 8)
{
    m_decoder.start_decode(fin);
    split_frame(c->m_atom_information, m_water_molecules, m_other_atoms);
}


void WaterReader::start()
{
    // Read file header
    int header_int[2];
    m_byte.decode(header_int, sizeof(int), 2);
    m_natoms   = header_int[0];
    m_nframes  = header_int[1];
    //int ISTART = header_int[2];
    //int NSAVC  = header_int[3];

    // Don't pass m_natoms since we are only permutation encoding the indicies
    // of the water molecules.
    m_adaptive_water = new SerialiseDecoder(m_compressor, &m_decoder,
                                            m_water_molecules.size());

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
    delete m_adaptive_water;
}


void WaterReader::next_frame_header(QuantisedFrame & qframe)
{
    unsigned int header_quant[3];
    m_byte.decode(header_quant, sizeof(int), 3);
    m_byte.decode(qframe.min_coord, sizeof(float), 3);
    m_byte.decode(qframe.max_coord, sizeof(float), 3);

    qframe.m_xquant = header_quant[0];
    qframe.m_yquant = header_quant[1];
    qframe.m_zquant = header_quant[2];
}


void WaterReader::next_frame_water(QuantisedFrame & qframe)
{
    OxygenGraph::readin(*m_adaptive_water, m_water_molecules, qframe);
}


void WaterReader::next_frame_other(QuantisedFrame & qframe)
{
    // Delta decoding on non water molecule atoms.
    for (size_t i = 0; i < m_other_atoms.size(); i++) {
        unsigned int idx = m_other_atoms[i];

        int prediction[3] = { 0, 0, 0 };
        if (m_previous_frame.natoms())
            for (int d = 0; d < 3; d++)
                prediction[d] = (int)m_previous_frame.at(idx, d);

        for (int d = 0; d < 3; d++)
            qframe.at(idx, d) = m_adaptive.decode_int() + prediction[d];
    }

    m_previous_frame = qframe;
}
