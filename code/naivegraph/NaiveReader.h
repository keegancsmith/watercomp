#pragma once

#include "FrameReader.h"

#include <cstdio>

class NaiveReader : public FrameReader
{
public:
    NaiveReader(FILE * fin);
    ~NaiveReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
};
