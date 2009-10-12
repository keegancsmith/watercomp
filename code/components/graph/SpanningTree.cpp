#include "SpanningTree.h"

#include <vector>
#include <climits>
#include <queue>

using std::vector;
using std::queue;


int dist_squared(const QuantisedFrame & frame, int a, int b)
{
    int d = 0;
    for (int i = 0; i < 3; i++) {
        int x = frame.quantised_frame[a*3 + i];
        int y = frame.quantised_frame[b*3 + i];
        int delta = x - y;
        d += (delta * delta);
    }
    return d;
}

// Find closest unseen atom
int find_closest_atom(const QuantisedFrame & frame, vector<bool> & seen,
                      int v)
{
    int closest = v == 0 ? 1 : 0;
    int closest_dist = INT_MAX;
    for (size_t e = 0; e < frame.natoms(); e++) {
        if (e == v || seen[e])
            continue;
        int d = dist_squared(frame, v, e);
        if (d < closest_dist) {
            closest = e;
            closest_dist = d;
        }
    }

    return closest;
}

Graph * spanning_tree(const QuantisedFrame & frame, int & root) {
    vector<bool> seen;
    seen.resize(frame.natoms());

    Graph * tree = new Graph(&frame, frame.natoms());

    root = 0;
    seen[root] = true;

    queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int i = 0; i < 2; i++) {
            int k = find_closest_atom(frame, seen, v);
            if (seen[k])
                break;
            seen[k] = true;
            tree->addEdge(v, k);
            q.push(k);
        }
    }

    return tree;
}
