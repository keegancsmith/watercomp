#include "TreeSerialiser.h"

#include "../arithmetic/AdaptiveModelEncoder.h"
#include "../arithmetic/AdaptiveModelDecoder.h"
#include "../quantiser/QuantisedFrame.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <queue>

using namespace std;


void encode_int(AdaptiveModelEncoder & enc, int i)
{
    char buf[12]; // Ints can only be 10 digits + sign + null
    sprintf(buf, "%d", i);
    enc.encode(buf);
}

int decode_int(AdaptiveModelDecoder & dec)
{
    string val = dec.decode();
    assert(val.size() < 12);
    return atoi(val.c_str());
}


// Serialises the tree in a BFS order
void serialise_tree(ArithmeticEncoder & ae, Graph * g, int root)
{
    QuantisedFrame * frame = (QuantisedFrame *)g->data;

    AdaptiveModelEncoder tree_encoder(&ae);
    AdaptiveModelEncoder err_encoder(&ae);
    AdaptiveModelEncoder index_encoder(&ae);

    // root is predicted to be at (0,0,0)
    unsigned int predictions[g->nVerticies][3];
    fill(predictions[root], predictions[root] + 3, 0);

    int atoms = frame->natoms();
    int count = 0;

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
	encode_int(tree_encoder, size);
	encode_int(index_encoder, index);
        for (int i = 0; i < 3; i++)
            encode_int(err_encoder, error[i]);

        // Add children verticies to process queue
	for (int i = 0; i < size; i++) {
	    int u = g->adjacent[v][i];
	    for (int j = 0; j < 3; j++)
		predictions[u][j] = frame->quantised_frame[3*v + i];
	    q.push(u);
	}
    }

    assert(count == frame->natoms());

    // Clean up
    tree_encoder.end_encode();
    err_encoder.end_encode();
    index_encoder.end_encode();
}


// Deserialise the tree in a BFS order
void deserialise_tree(ArithmeticDecoder & ad, QuantisedFrame & frame)
{
    AdaptiveModelDecoder tree_decoder(&ad);
    AdaptiveModelDecoder err_decoder(&ad);
    AdaptiveModelDecoder index_decoder(&ad);

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
        int size  = decode_int(tree_decoder);
        int index = decode_int(index_decoder);
        int error[3];
        for (int i = 0; i < 3; i++)
            error[i] = decode_int(err_decoder);

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
