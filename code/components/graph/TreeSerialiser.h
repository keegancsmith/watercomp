#pragma once

#include "Graph.h"
#include "../arithmetic/ArithmeticEncoder.h"

void serialise_tree(ArithmeticEncoder & ae, Graph * g, int root);
