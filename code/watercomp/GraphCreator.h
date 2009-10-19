#pragma once

#include "graph/Graph.h"
#include "quantiser/QuantisedFrame.h"
#include "splitter/WaterMolecule.h"

#include <vector>

Graph * create_oxygen_graph(const QuantisedFrame & qframe,
                            const std::vector<WaterMolecule> molecules);
