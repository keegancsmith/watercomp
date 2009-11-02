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

void inc_bit_offset(int & byte_offset, int & bit_offset)
{
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
        // Setup buf
        size_t size = (qframe.natoms() * dims[d] + 7) / 8;
        unsigned char buf[size];
        std::fill(buf, buf + size, 0);

        // Position in buf
        int byte_offset = 0;
        int bit_offset = 0;

        // Write dimension out into buf
        for (int i = 0; i < qframe.natoms(); i++) {
            unsigned int val = qframe.quantised_frame[3*i + d];
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
