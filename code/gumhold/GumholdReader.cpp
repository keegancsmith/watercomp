#include "GumholdReader.h"

#include "arithmetic/ByteDecoder.h"
#include "TreeSerialiser.h"

#include <cassert>

GumholdReader::GumholdReader(FILE * fin)
    : m_fin(fin)
{
}


void GumholdReader::start()
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


bool GumholdReader::next_frame(QuantisedFrame & qframe)
{
    ByteDecoder dec(&m_decoder);

    assert(qframe.natoms() == natoms());

    // Frame header
    unsigned int header_quant[3];
    dec.decode(header_quant, sizeof(int), 3);
    dec.decode(qframe.min_coord, sizeof(float), 3);
    dec.decode(qframe.max_coord, sizeof(float), 3);

    qframe.m_xquant = header_quant[0];
    qframe.m_yquant = header_quant[1];
    qframe.m_zquant = header_quant[2];

    deserialise_tree(m_decoder, qframe);

    return true;
}


void GumholdReader::end()
{
}
