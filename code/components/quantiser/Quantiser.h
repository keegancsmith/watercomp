#pragma once

#include "QuantisedFrame.h"

class Quantiser
{
public:
    Quantiser(const QuantisedFrame & qframe);

    void dequantise(const unsigned int * qpos, float * pos) const;
    void quantise(const float * pos, unsigned int * qpos) const;

private:
    unsigned int m_buckets[3];
    float m_range[3];
    float m_offset[3];
};
