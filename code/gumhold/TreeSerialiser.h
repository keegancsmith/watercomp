#pragma once

#include "graph/Graph.h"
#include "arithmetic/AdaptiveModelEncoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"
#include "Permutation.h"
#include "quantiser/QuantisedFrame.h"

// (frame, parent, vertex, prediction)
typedef void (gumhold_predictor) (const QuantisedFrame*,
                                  int, int, unsigned int*);

struct SerialiseEncoder
{
    SerialiseEncoder(Compressor * c, ArithmeticEncoder * ae, int natoms)
        : tree_encoder(ae), err_encoder(ae) {
        perm = PermutationWriter::get_writer(c, ae, natoms);
    }
    ~SerialiseEncoder() { delete perm; }
    AdaptiveModelEncoder tree_encoder;
    AdaptiveModelEncoder err_encoder;
    PermutationWriter * perm;
};

struct SerialiseDecoder
{
    SerialiseDecoder(Compressor * c, ArithmeticDecoder * ad, int natoms)
        : tree_decoder(ad), err_decoder(ad) {
        perm = PermutationReader::get_reader(c, ad, natoms);
    }
    ~SerialiseDecoder() { delete perm; }
    AdaptiveModelDecoder tree_decoder;
    AdaptiveModelDecoder err_decoder;
    PermutationReader * perm;
};

void serialise_tree(SerialiseEncoder & se, Graph * g, int root,
                    gumhold_predictor * pred);

void deserialise_tree(SerialiseDecoder & sd, QuantisedFrame & frame,
                      gumhold_predictor * pred);
