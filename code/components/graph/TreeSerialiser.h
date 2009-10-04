#pragma once

#include "Graph.h"
#include "../arithmetic/ArithmeticEncoder.h"
#include "../arithmetic/ArithmeticDecoder.h"
#include "../quantiser/QuantisedFrame.h"

void serialise_tree(ArithmeticEncoder & ae, Graph * g, int root);

void deserialise_tree(ArithmeticDecoder & ad, QuantisedFrame & frame);
