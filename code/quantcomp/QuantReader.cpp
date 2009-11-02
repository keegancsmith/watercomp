#include "QuantReader.h"

#include <cassert>

QuantReader::QuantReader(FILE * fin)
    : m_fin(fin)
{

}


void QuantReader::start()
{
    // Read file header
    int header_int[2];
    fread(header_int, sizeof(int), 2, m_fin);
    m_natoms   = header_int[0];
    m_nframes  = header_int[1];
    // int ISTART = header_int[2];
    // int NSAVC  = header_int[3];

    // double DELTA;
    // fread(&DELTA, sizeof(double), 1, m_fin);
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
        // Setup buf
        size_t size = (qframe.natoms() * dims[d] + 7) / 8;
        unsigned char buf[size];

        // Read into buf from file
        size_t read = 0;
        while (read < size)
            read += fread(buf + read, sizeof(char), size - read, m_fin);

        // Position in buf
        int byte_offset = 0;
        int bit_offset = 0;

        // Copy to qframe buffer
        for (int i = 0; i < natoms(); i++) {
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

            qframe.quantised_frame[3*i + d] = val;
        }
    }
}


void QuantReader::end()
{
}
