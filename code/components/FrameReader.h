#pragma once

#include "quantiser/QuantisedFrame.h"

class FrameReader {
public:
    virtual ~FrameReader() {};

    virtual void start() = 0;
    virtual bool next_frame(QuantisedFrame & qframe) = 0;
    virtual void end() = 0;

    int natoms() const { return m_natoms; }
    int nframes() const { return m_nframes; }

protected:
    int m_natoms, m_nframes;
};
