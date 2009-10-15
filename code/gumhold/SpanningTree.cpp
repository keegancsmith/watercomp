#include "SpanningTree.h"

#include <vector>
#include <climits>

using std::vector;


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

// Returns the closest atom to order[i] such that the atom is in order[k]
// where k < i
int find_closest_seen_atom(const QuantisedFrame & frame,
                           const vector<int> & order,
                           int i)
{
    int v = order[i];
    int closest = order[0];
    int closest_dist = dist_squared(frame, v, order[0]);
    for (int j = 1; j < i; j++) {
        int u = order[j];
        int d = dist_squared(frame, v, u);
        if (d < closest_dist) {
            closest = u;
            closest_dist = d;
        }
    }
    return closest;
}

Graph * spanning_tree(const QuantisedFrame & frame, int & root) {
    // We process atom order[i] in the ith turn
    vector<int> order(frame.natoms());

    // For now just assign the order as listed. Gumhold et al did this by
    // ordering either by x, y or z coord.
    for (int i = 0; i < frame.natoms(); i++)
        order[i] = i;
    root = order[0];

    Graph * tree = new Graph(&frame, frame.natoms());

    for (int i = 1; i < frame.natoms(); i++) {
        int v = find_closest_seen_atom(frame, order, i);
        int u = order[i];
        tree->addEdge(v, u);
    }

    return tree;
}
