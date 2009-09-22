#pragma once

#include "Graph.h"
#include "../quantiser/QFrame.h"

// Simple graph creator at first. Just create full connected graph of all the
// atoms.
Graph * createGraph(QFrame *);
