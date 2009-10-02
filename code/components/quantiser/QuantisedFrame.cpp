#include "QuantisedFrame.h"
#include <cassert>

using namespace std;

QuantisedFrame::QuantisedFrame(const Frame & frame,
               unsigned int x_subs, unsigned int y_subs, unsigned int z_subs)
    : m_xquant(x_subs), m_yquant(y_subs), m_zquant(z_subs)
{
    for(int d = 0; d < 3; ++d)
        min_coord[d] = max_coord[d] = frame.atom_data[d];

    for(size_t i = 1; i < frame.natoms(); ++i)
        for(int d = 0; d < 3; ++d)
        {
            min_coord[d] = min(frame.atom_data[3*i + d], min_coord[d]);
            max_coord[d] = max(frame.atom_data[3*i + d], max_coord[d]);
        }

    float range[3];

    for(int d = 0; d < 3; ++d)
        range[d] = max_coord[d] - min_coord[d];

    unsigned int buckets[3] = {1 << x_subs, 1 << y_subs, 1 << z_subs};
    
    for(size_t i = 0; i < frame.natoms(); ++i)
        for(int d = 0; d < 3; ++d)
        {
            float translated = frame.atom_data[3*i + d] - min_coord[d];
            float scaled = translated * buckets[d] / range[d];

            if(scaled < 0.5)
                scaled = 0.5;
            else if(scaled >= buckets[d] - 0.5)
                scaled = buckets[d] - 0.5;

            quantised_frame.push_back(scaled);

            assert(0 <= scaled && scaled <= buckets[d]);
        }
}

QuantisedFrame::QuantisedFrame(unsigned int size, unsigned int x_subs, unsigned int y_subs, unsigned int z_subs)
   : m_xquant(x_subs), m_yquant(y_subs), m_zquant(z_subs)
{
    quantised_frame.resize(3*size);
}

int QuantisedFrame::natoms() const {
    return (int) quantised_frame.size()/3;
}


Frame QuantisedFrame::toFrame() const {
    Frame frame(natoms());
    unsigned int buckets[3] = {1 << m_xquant, 1 << m_yquant, 1 << m_zquant};
    float range[3];
    for (int d = 0; d < 3; d++)
        range[d] = max_coord[d] - min_coord[d];


    // Approximate quantised points with floats
    for (int j = 0; j < natoms(); j++) {
        for (int d = 0; d < 3; d++) {
            int i = j * 3 + d;
            float approx = quantised_frame[i] + 0.5;
            float scaled = approx * range[d] / buckets[d];
            frame.atom_data[i] = scaled + min_coord[d];
        }
    }

    assert(quantised_frame.size() == frame.atom_data.size());

    return frame;
}