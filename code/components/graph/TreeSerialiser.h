#pragma once

#include "Graph.h"
#include "../arithmetic/ArithmeticEncoder.h"
#include "../arithmetic/ArithmeticDecoder.h"
#include "../quantiser/QuantisedFrame.h"
#include "../Permutation.h"


void serialise_tree(ArithmeticEncoder & ae, PermutationWriter & perm,
                    Graph * g, int root);

void deserialise_tree(ArithmeticDecoder & ad, PermutationReader & perm,
                      QuantisedFrame & frame);
