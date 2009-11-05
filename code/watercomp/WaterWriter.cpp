#include "WaterWriter.h"

#include "OxygenGraph.h"
#include "splitter/FrameSplitter.h"

using std::vector;

WaterWriter::WaterWriter(FILE * fout, const vector<AtomInformation> & pdb)
    : m_adaptive(&m_encoder), m_byte(&m_encoder)
{
    m_encoder.start_encode(fout);
    split_frame(pdb, m_water_molecules, m_other_atoms);
}


WaterWriter::~WaterWriter()
{
}


void WaterWriter::start(int atoms, int frames, int ISTART,
                        int NSAVC, double DELTA)
{
    // File header
    int header_int[2] = { atoms, frames }; //, ISTART, NSAVC };
    m_byte.encode(header_int, sizeof(int), 2);
    //enc.encode(&DELTA, sizeof(double), 1);

    m_adaptive_water = new SerialiseEncoder(&m_encoder, atoms);
}


void WaterWriter::next_frame(const QuantisedFrame& qframe)
{
    next_frame_header(qframe);
    next_frame_water(qframe);
    next_frame_other(qframe);
}


void WaterWriter::end()
{
    delete m_adaptive_water;
    m_encoder.end_encode();
}


void WaterWriter::next_frame_header(const QuantisedFrame & qframe)
{
    unsigned int header_quant[3] = {
        qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
    };
    m_byte.encode(header_quant, sizeof(unsigned int), 3);
    m_byte.encode(qframe.min_coord, sizeof(float), 3);
    m_byte.encode(qframe.max_coord, sizeof(float), 3);
}


void WaterWriter::next_frame_water(const QuantisedFrame & qframe)
{
    OxygenGraph oxygen_graph(qframe, m_water_molecules);
    oxygen_graph.writeout(*m_adaptive_water);
}


void WaterWriter::next_frame_other(const QuantisedFrame & qframe)
{
    // Write out other atoms per dimension
    size_t dims[3] = { qframe.m_xquant, qframe.m_yquant, qframe.m_zquant };
    for (int d = 0; d < 3; d++) {
        // Setup buf
        size_t size = (m_other_atoms.size() * dims[d] + 7) / 8;
        unsigned char buf[size];
        std::fill(buf, buf + size, 0);

        // Position in buf
        int byte_offset = 0;
        int bit_offset = 0;

        // Write dimension out into buf
        for (int i = 0; i < m_other_atoms.size(); i++) {
            unsigned int val = qframe.at(m_other_atoms[i], d);
            for (int j = 0; j < dims[d]; j++) {
                // Get and write j'th bit of val
                int bit = (val >> j) & 1;
                buf[byte_offset] |= (bit << bit_offset);

                // Adjust the bit and byte offset in buf
                bit_offset++;
                if (bit_offset == 8) {
                    byte_offset++;
                    bit_offset = 0;
                }
            }
        }

        // Encode buf to file
        for (int i = 0; i < size; i++)
            m_adaptive.encode_bytes(buf + i, 1);
    }
}
