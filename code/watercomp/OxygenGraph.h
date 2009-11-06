#pragma once

#include "WaterPredictor.h"

#include "arithmetic/AdaptiveModelDecoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "graph/Graph.h"
#include "Permutation.h"
#include "quantiser/QuantisedFrame.h"
#include "splitter/WaterMolecule.h"

#include <vector>


struct SerialiseEncoder
{
    SerialiseEncoder(ArithmeticEncoder * ae, int natoms)
        : tree_encoder(ae), err_encoder(ae) {
        perm = PermutationWriter::get_writer(ae, natoms);
    }
    ~SerialiseEncoder() { delete perm; }
    AdaptiveModelEncoder tree_encoder;
    AdaptiveModelEncoder err_encoder;
    PermutationWriter * perm;
};

struct SerialiseDecoder
{
    SerialiseDecoder(ArithmeticDecoder * ad, int natoms)
        : tree_decoder(ad), err_decoder(ad) {
        perm = PermutationReader::get_reader(ad, natoms);
    }
    ~SerialiseDecoder() { delete perm; }
    AdaptiveModelDecoder tree_decoder;
    AdaptiveModelDecoder err_decoder;
    PermutationReader * perm;
};


class OxygenGraph {
public:
    OxygenGraph(const QuantisedFrame & qframe,
                const std::vector<WaterMolecule> & waters);

    void writeout(SerialiseEncoder & enc);

    static void readin(SerialiseDecoder & dec,
                       const std::vector<WaterMolecule> & waters,
                       QuantisedFrame & qframe);

    // Some stats
    int nClusters;
    int nConstant;
    int nHydrogen;

private:
    Graph * create_oxygen_graph() const;
    Graph * create_spanning_tree(Graph * graph, int & root);
    void serialise(Graph * tree, int root, SerialiseEncoder & enc) const;

    int prediction_error(int water_idx,
                         WaterPredictor::Prediction & pred) const;
    bool create_component(Graph * graph, Graph * tree, int v, int r) const;

    WaterPredictor m_predictor;
    const QuantisedFrame & m_qframe;
    const std::vector<WaterMolecule> & m_waters;
};
