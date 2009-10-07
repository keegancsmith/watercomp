#include "TreeSerialiser.h"

#include "../arithmetic/AdaptiveModelEncoder.h"
#include "../arithmetic/AdaptiveModelDecoder.h"
#include "../quantiser/QuantisedFrame.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <climits>

using namespace std;


// Serialises the tree in a BFS order
void serialise_tree(ArithmeticEncoder & ae, PermutationWriter & perm,
                    Graph * g, int root)
{
    QuantisedFrame * frame = (QuantisedFrame *)g->data;

    AdaptiveModelEncoder tree_encoder(&ae);
    AdaptiveModelEncoder err_encoder(&ae);

    int atoms = frame->natoms();
    int count = 0;

    // root is predicted to be at (0,0,0)
    unsigned int predictions[atoms][3];
    fill(predictions[root], predictions[root] + 3, 0);

    queue<int> q;
    q.push(root);

    while(!q.empty()) {
        int v = q.front();
        q.pop();
        count++;

        // Calculate values
        int size = g->adjacent[v].size();
        int index = v;
        int error[3];
        for (int i = 0; i < 3; i++)
            error[i] = frame->quantised_frame[3*v + i] - predictions[v][i];

        // Assert if values are sensible
        assert(0 <= size && size <= 2);
        assert(0 <= index && index < atoms);

        // Write values to arithmetic encoder
        tree_encoder.encode_int(size);
        perm.next_index(index);
        for (int i = 0; i < 3; i++)
            err_encoder.encode_int(error[i]);

        // Add children verticies to process queue
        for (int i = 0; i < size; i++) {
            int u = g->adjacent[v][i];
            for (int j = 0; j < 3; j++)
                predictions[u][j] = frame->quantised_frame[3*v + j];
            q.push(u);
        }
    }

    assert(count == atoms);
    assert(g->nVerticies == atoms);
}


// Deserialise the tree in a BFS order
void deserialise_tree(ArithmeticDecoder & ad, PermutationReader & perm,
                      QuantisedFrame & frame)
{
    AdaptiveModelDecoder tree_decoder(&ad);
    AdaptiveModelDecoder err_decoder(&ad);

    int atoms = frame.natoms();
    int count = 0;

    queue<int> q;
    q.push(-1);

    while(!q.empty()) {
        int v = q.front();
        q.pop();
        count++;

        // Get position of parent and use it as the prediction
        unsigned int p[3];
        if (v == -1)
            p[0] = p[1] = p[2] = 0; // root is predicted to be at (0,0,0)
        else
            for (int i = 0; i < 3; i++)
                p[i] = frame.quantised_frame[3*v + i];

        // Read in values from file
        int size  = tree_decoder.decode_int();
        int index = perm.next_index();
        int error[3];
        for (int i = 0; i < 3; i++)
            error[i] = err_decoder.decode_int();

        // Check if they are sane
        assert(0 <= size && size <= 2);
        assert(0 <= index && index < atoms);

        // Values is the prediction + the error
        for (int i = 0; i < 3; i++)
            frame.quantised_frame[3*index + i] = p[i] + error[i];

        // Need to process each of its children
        for (int i = 0; i < size; i++)
            q.push(index);
    }

    assert(count == atoms);
}
