#include "SpanningTree.h"
#include "../quantiser/QuantisedFrame.h"

#include <vector>
#include <climits>
#include <queue>

using std::vector;
using std::queue;


int dist_squared(QuantisedFrame * frame, int a, int b) {
    int d = 0;
    for (int i = 0; i < 3; i++) {
        int x = frame->quantised_frame[a*3 + i];
        int y = frame->quantised_frame[b*3 + i];
        int delta = x - y;
        d += (delta * delta);
    }
    return d;
}

// Find closest unseen atom
int find_closest_atom(QuantisedFrame * frame, std::vector<int> * edges,
                      vector<bool> * seen, int v) {
    int closest = edges->at(0);
    int closest_dist = INT_MAX;
    for (size_t i = 0; i < edges->size(); i++) {
        int e = edges->at(i);
        if (seen->at(e))
            continue;
        int d = dist_squared(frame, v, e);
        if (d < closest_dist) {
            closest = d;
            closest_dist = d;
        }
    }

    return closest;
}

Graph * spanningTree(Graph * g, int & root) {
    vector<bool> seen;
    seen.reserve(g->nVerticies);

    QuantisedFrame * frame = (QuantisedFrame *)g->data;
    Graph * tree = new Graph(frame, g->nVerticies);

    root = 0;
    seen[root] = true;

    queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int i = 0; i < 2; i++) {
            int k = find_closest_atom(frame, g->adjacent + v, &seen, v);
            if (seen[k])
                break;
            seen[k] = true;
            tree->addEdge(v, k);
            q.push(k);
        }
    }
    
    return tree;
}
