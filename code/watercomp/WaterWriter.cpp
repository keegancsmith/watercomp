#include "WaterWriter.h"

#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/ByteEncoder.h"
#include "splitter/FrameSplitter.h"

using std::vector;

WaterWriter::WaterWriter(FILE * fout, const vector<AtomInformation> & pdb)
    : m_fout(fout)
{
    split_frame(pdb, m_water_molecules, m_other_atoms);
}


WaterWriter::~WaterWriter()
{
}


void WaterWriter::start(int atoms, int frames, int ISTART,
                        int NSAVC, double DELTA)
{
    m_encoder.start_encode(m_fout);
    ByteEncoder enc(&m_encoder);

    // File header
    int header_int[2] = { atoms, frames }; //, ISTART, NSAVC };
    enc.encode(header_int, sizeof(int), 2);
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
    ByteEncoder enc(&m_encoder);

    unsigned int header_quant[3] = {
        qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
    };
    enc.encode(header_quant, sizeof(unsigned int), 3);
    enc.encode(qframe.min_coord, sizeof(float), 3);
    enc.encode(qframe.max_coord, sizeof(float), 3);
}


void WaterWriter::next_frame_water(const QuantisedFrame & qframe)
{
    AdaptiveModelEncoder enc(&m_encoder);

    for (size_t i = 0; i < m_water_molecules.size(); i++) {
        WaterMolecule mol = m_water_molecules[i];
        unsigned int idx[3] = { mol.OH2_index, mol.H1_index, mol.H2_index };
        for (int j = 0; j < 3; j++)
            for (int d = 0; d < 3; d++)
                enc.encode_int(qframe.quantised_frame[3*idx[j] + d]);
    }
}


void WaterWriter::next_frame_other(const QuantisedFrame & qframe)
{
    AdaptiveModelEncoder enc(&m_encoder);

    for (size_t i = 0; i < m_other_atoms.size(); i++) {
        int idx = m_other_atoms[i];
        for (int d = 0; d < 3; d++)
            enc.encode_int(qframe.quantised_frame[3*idx + d]);
    }
}
