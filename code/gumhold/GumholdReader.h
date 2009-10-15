#pragma once

#include "FrameReader.h"
#include "arithmetic/ArithmeticDecoder.h"

#include <cstdio>

class GumholdReader : public FrameReader
{
public:
    GumholdReader(FILE * fin);
    ~GumholdReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
    ArithmeticDecoder m_decoder;
};
