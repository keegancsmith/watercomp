#include "devillersgandoin.h"

#include "Compressor.h"
#include "FrameReader.h"
#include "FrameWriter.h"
#include "Permutation.h"
#include "arithmetic/AdaptiveModelDecoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/ByteDecoder.h"
#include "arithmetic/ByteEncoder.h"
#include "quantiser/QuantisedFrame.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

class DGReader : public FrameReader
{
public:
    DGReader(FILE * fin) 
        : m_fin(fin), m_dec(&m_decoder), m_byte(&m_decoder) {}
    ~DGReader() {}

    void start() {
        m_decoder.start_decode(m_fin);

        // Read file header
        int header_int[2];
        m_byte.decode(header_int, sizeof(int), 2);
        m_natoms   = header_int[0];
        m_nframes  = header_int[1];
        // Commented out values which are not used
        //int ISTART = header_int[2];
        //int NSAVC  = header_int[3];

        //double DELTA;
        //dec.decode(&DELTA, sizeof(double), 1);
    }

    bool next_frame(QuantisedFrame & qframe) {
        if (m_perm == 0)
            m_perm = PermutationReader::get_reader(&m_decoder, natoms());
        else
            m_perm->reset();

        assert(qframe.natoms() == natoms());

        // Frame header
        unsigned int header_quant[3];
        m_byte.decode(header_quant, sizeof(int), 3);
        m_byte.decode(qframe.min_coord, sizeof(float), 3);
        m_byte.decode(qframe.max_coord, sizeof(float), 3);

        qframe.m_xquant = header_quant[0];
        qframe.m_yquant = header_quant[1];
        qframe.m_zquant = header_quant[2];

        // Get size of list
        unsigned int size;
        m_byte.decode(&size, sizeof(unsigned int), 1);

        // Read in data
        vector<coord_t> encoded;
        encoded.resize(size);
        for (size_t i = 0; i < size; i++)
            encoded[i] = m_dec.decode_int();

        // Read in permutation
        vector<int> perm;
        perm.resize(natoms());
        for (int i = 0; i < natoms(); i++)
            perm[i] = m_perm->next_index();

        // Do the reverse devillers and gandoin transformation
        unsigned int bits = *max_element(header_quant, header_quant+3);
        vector<point_t> decoded = decode(encoded, bits);

        // Update qframe
        for (int i = 0; i < natoms(); i++) {
            int idx = perm[i];
            for (int d = 0; d < 3; d++)
                qframe.quantised_frame[idx*3+d] = decoded[i].coords[d];
        }

        return true;
    }

    void end() {
        delete m_perm;
    }

private:
    FILE * m_fin;
    ArithmeticDecoder m_decoder;
    AdaptiveModelDecoder m_dec;
    ByteDecoder m_byte;
    PermutationReader * m_perm;
};


class DGWriter : public FrameWriter
{
public:
    DGWriter(FILE * fout)
        : m_fout(fout), m_enc(&m_encoder), m_byte(&m_encoder) {}
    ~DGWriter() {}

    void start(int atoms, int frames, int ISTART = 0,
               int NSAVC = 1, double DELTA = 0.0) {
        m_encoder.start_encode(m_fout);
        ByteEncoder enc(&m_encoder);

        // File header
        int header_int[2] = { atoms, frames }; //, ISTART, NSAVC };
        enc.encode(header_int, sizeof(int), 2);
        //enc.encode(&DELTA, sizeof(double), 1);
    }

    void next_frame(const QuantisedFrame & qframe) {
        if (m_perm == 0)
            m_perm = PermutationWriter::get_writer(&m_encoder,
                                                   qframe.natoms());
        else
            m_perm->reset();
        
        // Frame header
        unsigned int header_quant[3] = {
            qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
        };
        m_byte.encode(header_quant, sizeof(unsigned int), 3);
        m_byte.encode(qframe.min_coord, sizeof(float), 3);
        m_byte.encode(qframe.max_coord, sizeof(float), 3);


        // Get data needed for transform
        unsigned int bits = *max_element(header_quant, header_quant+3);
        vector<point_t> points;
        points.resize(qframe.natoms());
        for (size_t i = 0; i < qframe.natoms(); i++) {
            points[i].index = i;
            for (int d = 0; d < 3; d++)
                points[i].coords[d] = qframe.quantised_frame[i*3 + d];
        }


        // Do the devillers and gandoin transformation
        vector<int> perm;
        vector<coord_t> encoded = encode(points, perm, bits);
        unsigned int size = encoded.size();
        assert(perm.size() == qframe.natoms());

        // Write out data
        m_byte.encode(&size, sizeof(unsigned int), 1);
        for (size_t i = 0; i < size; i++)
            m_enc.encode_int(encoded[i]);
        for (size_t i = 0; i < qframe.natoms(); i++)
            m_perm->next_index(perm[i]);
    }

    void end() {
        delete m_perm;
        m_encoder.end_encode();
    }

private:
    FILE * m_fout;
    ArithmeticEncoder m_encoder;
    AdaptiveModelEncoder m_enc;
    ByteEncoder m_byte;
    PermutationWriter * m_perm;
};


class DGCompressor : public Compressor
{
public:
    FrameReader * frame_reader(FILE * fin) {
        return new DGReader(fin);
    }

    FrameWriter * frame_writer(FILE * fout) {
        return new DGWriter(fout);
    }
};


int main(int argc, char ** argv)
{
    DGCompressor c;
    return do_main(c, argc, argv);
}
