#include "NaiveReader.h"

#include "arithmetic/ArithmeticDecoder.h"
#include "graph/TreeSerialiser.h"

#include <cassert>

NaiveReader::NaiveReader(FILE * fin)
    : m_fin(fin)
{

}


void NaiveReader::start()
{
    // Read file header
    int header_int[4];
    fread(header_int, sizeof(int), 4, m_fin);
    m_natoms   = header_int[0];
    m_nframes  = header_int[1];
    int ISTART = header_int[2];
    int NSAVC  = header_int[3];

    double DELTA;
    fread(&DELTA, sizeof(double), 1, m_fin);

    // TODO atm we are ignoring most of these values!
}


bool NaiveReader::next_frame(QuantisedFrame & qframe)
{
    assert(qframe.natoms() == natoms());

    // Frame header
    unsigned int header_quant[3];
    fread(header_quant, sizeof(int), 3, m_fin);
    fread(qframe.min_coord, sizeof(float), 3, m_fin);
    fread(qframe.max_coord, sizeof(float), 3, m_fin);

    qframe.m_xquant = header_quant[0];
    qframe.m_yquant = header_quant[1];
    qframe.m_zquant = header_quant[2];

    ArithmeticDecoder ad;
    ad.start_decode(m_fin);
    deserialise_tree(ad, qframe);
}


void NaiveReader::end()
{
}
