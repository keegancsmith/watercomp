#include "WaterPredictor.h"

#include <cassert>
#include <cmath>
#include <climits>

WaterPredictor::WaterPredictor(const QuantisedFrame & qframe)
    : m_qframe(qframe), m_quantiser(qframe)
{
}


WaterPredictor::Prediction
WaterPredictor::predict_constant(const WaterMolecule & mol) const
{
    WaterPredictor::Prediction pred;

    if (mol.OH2_index == INT_MAX) {
        for (int d = 0; d < 3; d++)
            pred.O[d] = pred.H1[d] = pred.H2[d] = 0;
        return pred;
    }

    for (int d = 0; d < 3; d++) {
        pred.O[d]  = m_qframe.at(mol.OH2_index, d);
        pred.H1[d] = m_qframe.at(mol.H1_index,  d);
        pred.H2[d] = m_qframe.at(mol.H2_index,  d);
    }

    return pred;
}


WaterPredictor::Prediction
WaterPredictor::predict_along_h1(const WaterMolecule & mol) const
{
    return predict(mol, true);
}


WaterPredictor::Prediction
WaterPredictor::predict_along_h2(const WaterMolecule & mol) const
{
    return predict(mol, false);
}


WaterPredictor::Prediction
WaterPredictor::predict(const WaterMolecule & parent, bool along_h1) const
{
    WaterPredictor::Prediction pred;

    if (parent.OH2_index == INT_MAX) {
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


    // TODO We can do a better prediction for Hydrogens
    float pred_H1[3];
    float pred_H2[3];
    for (int d = 0; d < 3; d++)
        pred_H1[d] = pred_H2[d] = pred_O[d];


    // Convert back to the quantised positions
    m_quantiser.quantise(pred_O, pred.O);
    m_quantiser.quantise(pred_H1, pred.H1);
    m_quantiser.quantise(pred_H2, pred.H2);

    return pred;
}


void WaterPredictor::dequantise(int idx, float * pos) const
{
    unsigned int qpos[3];
    for (int d = 0; d < 3; d++)
        qpos[d] = m_qframe.at(idx, d);
    return m_quantiser.dequantise(qpos, pos);
}
