#pragma once

#include "FrameReader.h"
#include "TreeSerialiser.h"
#include "arithmetic/ArithmeticDecoder.h"

#include <cstdio>

class GumholdReader : public FrameReader
{
public:
    GumholdReader(FILE * fin, gumhold_predictor * pred);
    ~GumholdReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
    ArithmeticDecoder m_decoder;
    SerialiseDecoder * m_dec;
    gumhold_predictor * m_pred;
};
