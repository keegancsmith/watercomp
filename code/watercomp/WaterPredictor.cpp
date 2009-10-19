#include "WaterPredictor.h"

#include <cassert>
#include <cmath>

WaterPredictor::WaterPredictor(const QuantisedFrame & qframe)
    : m_qframe(qframe)
{
    m_buckets[0] = 1 << qframe.m_xquant;
    m_buckets[1] = 1 << qframe.m_yquant;
    m_buckets[2] = 1 << qframe.m_zquant;

    for (int d = 0; d < 3; d++)
        m_range[d] = qframe.max_coord[d] - qframe.min_coord[d];
}


WaterPredictor::Prediction
WaterPredictor::predict_along_h1(const WaterMolecule & mol)
{
    return predict(mol, true);
}


WaterPredictor::Prediction
WaterPredictor::predict_along_h2(const WaterMolecule & mol)
{
    return predict(mol, false);
}


WaterPredictor::Prediction
WaterPredictor::predict(const WaterMolecule & parent, bool along_h1)
{
    WaterPredictor::Prediction pred;

    if (parent.OH2_index == -1) {
        for (int d = 0; d < 3; d++)
            pred.O[d] = pred.H1[d] = pred.H2[d] = 0;
        return pred;
    }

    int O_idx = parent.OH2_index;
    int H_idx = along_h1 ? parent.H1_index : parent.H2_index;

    float O_pos[3], H_pos[3];
    dequantise(O_idx, O_pos);
    dequantise(H_idx, H_pos);

    // These predictions are simplified. This is to make the code more concise
    // (sacrifising very slight error). Comments will explain the
    // simplifications.

    // We need the O-H unit vector to predict where the Oxygen is.
    float O_H_vec[3];
    float O_H_len = 0;
    float O_H_unit[3];
    for (int d = 0; d < 3; d++)
        O_H_vec[d] = H_pos[d] - O_pos[d];
    for (int d = 0; d < 3; d++)
        O_H_len += (O_H_vec[d] * O_H_vec[d]);
    O_H_len = sqrtf(O_H_len);
    for (int d = 0; d < 3; d++)
        O_H_unit[d] = O_H_vec[d] / O_H_len;


    // In the model the angle between O-H to the predicted Oxygen is 6+-20
    // degrees. Assume it is 0 degrees. The distance between O-O pair is then
    // 2.976 angstroms.
    float pred_O[3];
    for (int d = 0; d < 3; d++)
        pred_O[d] = O_pos[d] + 2.976 * O_H_unit[d];


    // TODO We can do a better prediction for H
    float pred_H1[3];
    float pred_H2[3];
    for (int d = 0; d < 3; d++)
        pred_H1[d] = pred_H2[d] = pred_O[d];


    // Convert back to the quantised positions
    quantise(pred_O, pred.O);
    quantise(pred_H1, pred.H1);
    quantise(pred_H2, pred.H2);

    return pred;
}


void WaterPredictor::dequantise(int idx, float * pos)
{
    for (int d = 0; d < 3; d++) {
        float approx = m_qframe.at(idx, d) + 0.5;
        float scaled = approx * m_range[d] / m_buckets[d];
        pos[d] = scaled + m_qframe.min_coord[d];
    }
}


void WaterPredictor::quantise(float * pos, unsigned int * qpos)
{
    for(int d = 0; d < 3; ++d) {
        float translated = pos[d] - m_qframe.min_coord[d];
        float scaled = translated * m_buckets[d] / m_range[d];

        if(scaled < 0.5)
            scaled = 0.5;
        else if(scaled >= m_buckets[d] - 0.5)
            scaled = m_buckets[d] - 0.5;

        qpos[d] = scaled;

        assert(0 <= scaled && scaled <= m_buckets[d]);
    }
}
    // if (parent.OH2_index == -1) {
    //     for (int d = 0; d < 3; d++)
    //         pred.p1.O[d] = pred.p1.H1[d] = pred.p1.H2[d] = 0;
    //     pred.p2 = pred.p1;
    //     return pred;
    // }
