#include "QuantWriter.h"

#include <cassert>

QuantWriter::QuantWriter(FILE * fout)
    : m_fout(fout)
{
}

QuantWriter::~QuantWriter()
{
}

void QuantWriter::start(int atoms, int frames, int ISTART,
                        int NSAVC, double DELTA)
{
    // File header
    int header_int[2] = { atoms, frames }; //, ISTART, NSAVC };
    fwrite(header_int, sizeof(int), 2, m_fout);
    //fwrite(&DELTA, sizeof(double), 1, m_fout);
}

void QuantWriter::next_frame(const QuantisedFrame& qframe)
{
    // Frame header
    unsigned int header_quant[3] = {
        qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
    };
    fwrite(header_quant, sizeof(unsigned int), 3, m_fout);
    fwrite(qframe.min_coord, sizeof(float), 3, m_fout);
    fwrite(qframe.max_coord, sizeof(float), 3, m_fout);

    // Write out qframe per dimension
    size_t dims[3] = { qframe.m_xquant, qframe.m_yquant, qframe.m_zquant };
    for (int d = 0; d < 3; d++) {
        assert(8 <= dims[d]);
        assert(dims[d] <= sizeof(unsigned int) * 8);
        assert(dims[d] % 8 == 0);

        // Write dimension out into buf
        size_t bytes = dims[d] / 8;
        size_t size = qframe.natoms() * bytes;
        unsigned char buf[size];
        for (int i = 0; i < qframe.natoms(); i++) {
            size_t offset = i * bytes;

            unsigned int val = qframe.quantised_frame[3*i + d];
            for (size_t j = 0; j < bytes; j++)
                buf[offset + j] = (val >> (8*j)) & 0xff;
        }

        // Write buf to file
        size_t written = 0;
        while (written < size)
            written += fwrite(buf + written, sizeof(char),
                              size - written, m_fout);
    }
}

void QuantWriter::end()
{
}
