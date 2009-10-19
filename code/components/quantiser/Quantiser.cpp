#include "Quantiser.h"

#include <cassert>

Quantiser::Quantiser(const QuantisedFrame & qframe)
{
    m_buckets[0] = 1 << qframe.m_xquant;
    m_buckets[1] = 1 << qframe.m_yquant;
    m_buckets[2] = 1 << qframe.m_zquant;

    for (int d = 0; d < 3; d++) {
        m_range[d] = qframe.max_coord[d] - qframe.min_coord[d];
        m_offset[d] = qframe.min_coord[d];
    }
}


void Quantiser::dequantise(const unsigned int * qpos, float * pos) const
{
    for (int d = 0; d < 3; d++) {
        float approx = qpos[d] + 0.5;
        float scaled = approx * m_range[d] / m_buckets[d];
        pos[d] = scaled + m_offset[d];
    }
}


void Quantiser::quantise(const float * pos, unsigned int * qpos) const
{
    for(int d = 0; d < 3; ++d) {
        float translated = pos[d] - m_offset[d];
        float scaled = translated * m_buckets[d] / m_range[d];

        if(scaled < 0.5)
            scaled = 0.5;
        else if(scaled >= m_buckets[d] - 0.5)
            scaled = m_buckets[d] - 0.5;

        qpos[d] = scaled;

        assert(0 <= scaled && scaled <= m_buckets[d]);
    }
}
