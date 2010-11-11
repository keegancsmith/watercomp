#pragma once

#include "OxygenGraph.h"

#include "Compressor.h"
#include "FrameWriter.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/ByteEncoder.h"
#include "splitter/WaterMolecule.h"
#include "quantiser/QuantisedFrame.h"

#include <cstdio>
#include <vector>

class WaterWriter : public FrameWriter
{
public:
    WaterWriter(FILE * fout, Compressor * compressor);
    ~WaterWriter();

    void start(int atoms, int frames, int ISTART = 0,
               int NSAVC = 1, double DELTA = 0.0);
    void next_frame(const QuantisedFrame & qframe);
    void end();

private:
    void next_frame_header(const QuantisedFrame & qframe);
    void next_frame_water(const QuantisedFrame & qframe);
    void next_frame_other(const QuantisedFrame & qframe);

    ArithmeticEncoder m_encoder;
    AdaptiveModelEncoder m_adaptive;
    SerialiseEncoder * m_adaptive_water;
    ByteEncoder m_byte;
    Compressor * m_compressor;

    // Some stats
    std::vector<int> nClusters;
    std::vector<int> nConstant;
    std::vector<int> nHydrogen;

    std::vector<WaterMolecule> m_water_molecules;
    std::vector<unsigned int> m_other_atoms;
};
