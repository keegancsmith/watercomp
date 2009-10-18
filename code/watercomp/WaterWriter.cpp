#include "WaterWriter.h"
#include "arithmetic/ByteEncoder.h"

WaterWriter::WaterWriter(FILE * fout)
    : m_fout(fout)
{
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
    ByteEncoder enc(&m_encoder);

    // Frame header
    unsigned int header_quant[3] = {
        qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
    };
    enc.encode(header_quant, sizeof(unsigned int), 3);
    enc.encode(qframe.min_coord, sizeof(float), 3);
    enc.encode(qframe.max_coord, sizeof(float), 3);
}


void WaterWriter::end()
{
    m_encoder.end_encode();
}
