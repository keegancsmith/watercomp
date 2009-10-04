#pragma once

#include "Graph.h"
#include "../quantiser/QuantisedFrame.h"

// Simple graph creator at first. Just create fully connected graph of all the
// atoms.
Graph * create_graph(QuantisedFrame *);
