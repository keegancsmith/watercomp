#include "WaterWriter.h"

#include "OxygenGraph.h"
#include "splitter/FrameSplitter.h"

using std::vector;

WaterWriter::WaterWriter(FILE * fout, const vector<AtomInformation> & pdb)
    : m_adaptive(&m_encoder), m_adaptive_water(&m_encoder), m_byte(&m_encoder)
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
}


void WaterWriter::next_frame(const QuantisedFrame& qframe)
{
    next_frame_header(qframe);
    next_frame_water(qframe);
    next_frame_other(qframe);
}


void WaterWriter::end()
{
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
    oxygen_graph.writeout(m_adaptive_water);
}


void WaterWriter::next_frame_other(const QuantisedFrame & qframe)
{
    for (size_t i = 0; i < m_other_atoms.size(); i++) {
        int idx = m_other_atoms[i];
        for (int d = 0; d < 3; d++)
            m_adaptive.encode_int(qframe.at(idx, d));
    }
}
