#include "WaterReader.h"

#include "splitter/FrameSplitter.h"
#include "OxygenGraph.h"

#include <cassert>

using std::vector;

WaterReader::WaterReader(FILE * fin, const vector<AtomInformation> & pdb)
    : m_adaptive(&m_decoder), m_byte(&m_decoder)
{
    m_decoder.start_decode(fin);
    split_frame(pdb, m_water_molecules, m_other_atoms);
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

    m_adaptive_water = new SerialiseDecoder(&m_decoder, m_natoms);

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
    // Read in qframe per dimension
    size_t dims[3] = { qframe.m_xquant, qframe.m_yquant, qframe.m_zquant };
    for (int d = 0; d < 3; d++) {
        // Setup buf
        size_t size = (m_other_atoms.size() * dims[d] + 7) / 8;
        unsigned char buf[size];

        // Read into buf from file
        for (int i = 0; i < size; i++)
            m_adaptive.decode_bytes(buf + i);

        // Position in buf
        int byte_offset = 0;
        int bit_offset = 0;

        // Copy to qframe buffer
        for (int i = 0; i < m_other_atoms.size(); i++) {
            unsigned int val = 0;

            for (int j = 0; j < dims[d]; j++) {
                // Get and write j'th bit of val
                int bit = (buf[byte_offset] >> bit_offset) & 1;
                val |= (bit << j);

                // Adjust the bit and byte offset in buf
                bit_offset++;
                if (bit_offset == 8) {
                    byte_offset++;
                    bit_offset = 0;
                }
            }

            qframe.at(m_other_atoms[i], d) = val;
        }
    }
}
