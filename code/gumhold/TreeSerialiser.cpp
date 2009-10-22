#include "TreeSerialiser.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <climits>

using namespace std;


// Serialises the tree in a BFS order
void serialise_tree(SerialiseEncoder & se, Graph * g, int root,
                    gumhold_predictor * pred)
{
    QuantisedFrame * frame = (QuantisedFrame *)g->data;
    se.perm->reset();

    int atoms = frame->natoms();
    int count = 0;

    unsigned int predictions[atoms][3];
    pred(frame, -1, -1, predictions[root]);

    vector<int> parent(atoms);
    parent[root] = -1;

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
        assert(0 <= size && size < atoms);
        assert(0 <= index && index < atoms);

        // Write values to arithmetic encoder
        se.tree_encoder.encode_int(size);
        se.perm->next_index(index);
        for (int i = 0; i < 3; i++)
            se.err_encoder.encode_int(error[i]);

        // Calculate prediction for children
        unsigned int p[3];
        pred(frame, v, parent[v], p);

        // Add children verticies to process queue
        for (int i = 0; i < size; i++) {
            int u = g->adjacent[v][i];
            parent[u] = v;
            for (int j = 0; j < 3; j++)
                predictions[u][j] = p[j];
            q.push(u);
        }
    }

    assert(count == atoms);
    assert(g->nVerticies == atoms);
}


// Deserialise the tree in a BFS order
void deserialise_tree(SerialiseDecoder & sd, QuantisedFrame & frame,
                      gumhold_predictor * pred)
{
    sd.perm->reset();
    int atoms = frame.natoms();
    int count = 0;

    vector<int> parent(atoms);
    queue<int> q;
    q.push(-1);

    while(!q.empty()) {
        int v = q.front();
        q.pop();
        count++;

        // Predict position from parent and grandparent
        int v_parent = v == -1 ? -1 : parent[v];
        unsigned int p[3];
        pred(&frame, v, v_parent, p);

        // Read in values from file
        int size  = sd.tree_decoder.decode_int();
        int index = sd.perm->next_index();
        int error[3];
        for (int i = 0; i < 3; i++)
            error[i] = sd.err_decoder.decode_int();

        // Check if they are sane
        assert(0 <= size && size < atoms);
        assert(0 <= index && index < atoms);

        // Value is the prediction + the error
        for (int i = 0; i < 3; i++)
            frame.quantised_frame[3*index + i] = p[i] + error[i];

        // Need to process each of its children
        for (int i = 0; i < size; i++)
            q.push(index);

        parent[index] = v;
    }

    assert(count == atoms);
}
