#pragma once

#include "FrameWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "splitter/WaterMolecule.h"
#include "quantiser/QuantisedFrame.h"

#include <cstdio>
#include <vector>

class WaterWriter : public FrameWriter
{
public:
    WaterWriter(FILE * fout, const std::vector<AtomInformation> & atom_info);
    ~WaterWriter();

    void start(int atoms, int frames, int ISTART = 0,
               int NSAVC = 1, double DELTA = 0.0);
    void next_frame(const QuantisedFrame & qframe);
    void end();

private:
    void next_frame_header(const QuantisedFrame & qframe);
    void next_frame_water(const QuantisedFrame & qframe);
    void next_frame_other(const QuantisedFrame & qframe);

    FILE * m_fout;
    ArithmeticEncoder m_encoder;

    std::vector<WaterMolecule> m_water_molecules;
    std::vector<unsigned int> m_other_atoms;
};
