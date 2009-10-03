#pragma once

#include "FrameWriter.h"
#include "quantiser/QuantisedFrame.h"

#include <cstdio>

class NaiveWriter : public FrameWriter
{
public:
    NaiveWriter(FILE * fout);
    ~NaiveWriter();

    void start(int atoms, int frames, int ISTART = 0,
               int NSAVC = 1, double DELTA = 0.0);
    void next_frame(const QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fout;
};
