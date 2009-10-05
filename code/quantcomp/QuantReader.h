#pragma once

#include "FrameReader.h"

#include <cstdio>

class QuantReader : public FrameReader
{
public:
    QuantReader(FILE * fin);
    ~QuantReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
};
