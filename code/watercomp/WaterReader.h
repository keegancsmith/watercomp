#pragma once

#include "FrameReader.h"
#include "arithmetic/ArithmeticDecoder.h"

#include <cstdio>

class WaterReader : public FrameReader
{
public:
    WaterReader(FILE * fin);
    ~WaterReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
    ArithmeticDecoder m_decoder;
};
