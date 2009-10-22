#pragma once

#include "FrameWriter.h"
#include "TreeSerialiser.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "quantiser/QuantisedFrame.h"

#include <cstdio>

class GumholdWriter : public FrameWriter
{
public:
    GumholdWriter(FILE * fout, gumhold_predictor * pred);
    ~GumholdWriter();

    void start(int atoms, int frames, int ISTART = 0,
               int NSAVC = 1, double DELTA = 0.0);
    void next_frame(const QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fout;
    ArithmeticEncoder m_encoder;
    SerialiseEncoder * m_enc;
    gumhold_predictor * m_pred;
};
