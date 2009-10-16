#pragma once

#include "graph/Graph.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "quantiser/QuantisedFrame.h"

// (frame, parent, vertex, prediction)
typedef void (gumhold_predictor) (const QuantisedFrame*,
                                  int, int, unsigned int*);

void serialise_tree(ArithmeticEncoder & ae, Graph * g, int root,
                    gumhold_predictor * pred);

void deserialise_tree(ArithmeticDecoder & ad, QuantisedFrame & frame,
                      gumhold_predictor * pred);
