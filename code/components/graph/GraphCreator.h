#pragma once

#include "Graph.h"
#include "../quantiser/QuantisedFrame.h"

// Simple graph creator at first. Just create full connected graph of all the
// atoms.
Graph * createGraph(QuantisedFrame *);
