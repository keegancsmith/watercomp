#pragma once

#include "FrameReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "splitter/WaterMolecule.h"

#include <cstdio>
#include <vector>

class WaterReader : public FrameReader
{
public:
    WaterReader(FILE * fin, const std::vector<AtomInformation> & atom_info);
    ~WaterReader() {}

    void start();
    bool next_frame(QuantisedFrame & qframe);
    void end();

private:
    FILE * m_fin;
    ArithmeticDecoder m_decoder;

    void next_frame_header(QuantisedFrame & qframe);
    void next_frame_water(QuantisedFrame & qframe);
    void next_frame_other(QuantisedFrame & qframe);

    std::vector<WaterMolecule> m_water_molecules;
    std::vector<unsigned int> m_other_atoms;
};
