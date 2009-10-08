#pragma once

#include "Graph.h"
#include "../quantiser/QuantisedFrame.h"

// Greedily creates a spanning tree
Graph * spanning_tree(const QuantisedFrame & f, int & root);
