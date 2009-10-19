#pragma once

#include "arithmetic/AdaptiveModelDecoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "graph/Graph.h"
#include "quantiser/QuantisedFrame.h"
#include "splitter/WaterMolecule.h"

#include <vector>

class OxygenGraph {
public:
    OxygenGraph(const QuantisedFrame & qframe,
                const std::vector<WaterMolecule> & waters);

    void writeout(AdaptiveModelEncoder & enc) const;

    static void readin(AdaptiveModelDecoder & dec,
                       const std::vector<WaterMolecule> & waters,
                       QuantisedFrame & qframe);

private:
    Graph * create_oxygen_graph() const;
    Graph * create_spanning_tree(Graph * graph, int & root) const;
    void serialise(Graph * tree, int root, AdaptiveModelEncoder & enc) const;

    const QuantisedFrame & m_qframe;
    const std::vector<WaterMolecule> & m_waters;
};
