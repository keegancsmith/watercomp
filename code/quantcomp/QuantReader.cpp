#include "QuantReader.h"

#include <cassert>

QuantReader::QuantReader(FILE * fin)
    : m_fin(fin)
{

}


void QuantReader::start()
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


bool QuantReader::next_frame(QuantisedFrame & qframe)
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

    // Read in qframe per dimension
    size_t dims[3] = { qframe.m_xquant, qframe.m_yquant, qframe.m_zquant };
    for (int d = 0; d < 3; d++) {
        assert(8 <= dims[d]);
        assert(dims[d] <= sizeof(unsigned int) * 8);
        assert(dims[d] % 8 == 0);

        // Read in from file
        size_t bytes = dims[d] / 8;
        size_t size = natoms() * bytes;
        size_t read = 0;
        unsigned char buf[size];
        while (read < size)
            read += fread(buf + read, sizeof(char), size - read, m_fin);

        // Copy to qframe buffer
        for (int i = 0; i < natoms(); i++) {
            size_t offset = i * bytes;

            unsigned int val = 0;
            for (size_t j = 0; j < bytes; j++)
                val |= (buf[offset + j] << (8*j));

            qframe.quantised_frame[3*i + d] = val;
        }
    }
}


void QuantReader::end()
{
}
