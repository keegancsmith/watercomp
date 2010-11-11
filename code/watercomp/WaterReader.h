#pragma once

#include "OxygenGraph.h"

#include "Compressor.h"
#include "FrameReader.h"
#include "arithmetic/AdaptiveModelDecoder.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/ByteDecoder.h"
#include "splitter/WaterMolecule.h"

#include <cstdio>
#include <vector>

class WaterReader : public FrameReader
{
public:
    WaterReader(FILE * fin, Compressor * compressor);
    ~WaterReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    ArithmeticDecoder m_decoder;
    AdaptiveModelDecoder m_adaptive;
    SerialiseDecoder * m_adaptive_water;
    ByteDecoder m_byte;
    Compressor * m_compressor;

    void next_frame_header(QuantisedFrame & qframe);
    void next_frame_water(QuantisedFrame & qframe);
    void next_frame_other(QuantisedFrame & qframe);

    std::vector<WaterMolecule> m_water_molecules;
    std::vector<unsigned int> m_other_atoms;
};
