#pragma once

#include "graph/Graph.h"
#include "quantiser/QuantisedFrame.h"
#include "TreeSerialiser.h"

// Greedily creates a spanning tree
Graph * spanning_tree(const QuantisedFrame & f, int & root,
                      gumhold_predictor * pred);
