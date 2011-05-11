#pragma once

#include "FrameReader.h"
#include "TreeSerialiser.h"
#include "arithmetic/ArithmeticDecoder.h"

#include <cstdio>

class GumholdReader : public FrameReader
{
public:
    GumholdReader(FILE * fin, Compressor * compressor,
                  gumhold_predictor * pred);
    ~GumholdReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
    ArithmeticDecoder m_decoder;
    SerialiseDecoder * m_dec;
    Compressor * m_compressor;
    gumhold_predictor * m_pred;
};
