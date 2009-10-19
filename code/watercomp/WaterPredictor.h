#pragma once

#include "quantiser/QuantisedFrame.h"
#include "quantiser/Quantiser.h"
#include "splitter/WaterMolecule.h"

class WaterPredictor
{
public:
    struct Prediction
    {
        unsigned int O[3], H1[3], H2[3];
    };

    WaterPredictor(const QuantisedFrame & qframe);

    Prediction predict_along_h1(const WaterMolecule & mol);
    Prediction predict_along_h2(const WaterMolecule & mol);

private:
    Prediction predict(const WaterMolecule & mol, bool along_h1);
    void dequantise(int idx, float * pos);

    const QuantisedFrame & m_qframe;
    Quantiser m_quantiser;
};
