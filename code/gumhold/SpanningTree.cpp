#include "SpanningTree.h"

#include <algorithm>
#include <climits>
#include <vector>

using std::vector;


int dist_squared(const QuantisedFrame & frame, int a, unsigned int * p)
{
    int d = 0;
    for (int i = 0; i < 3; i++) {
        int x = frame.quantised_frame[a*3 + i];
        int y = p[i];
        int delta = x - y;
        d += (delta * delta);
    }
    return d;
}


// Returns the closest atom to order[i] such that the atom is in order[k]
// where k < i
int find_closest_seen_atom(const QuantisedFrame & frame,
                           const vector<int> & order,
                           unsigned int * predictions,
                           int i)
{
    int v = order[i];
    int closest = order[0];
    int closest_dist = dist_squared(frame, v, predictions);
    for (int j = 1; j < i; j++) {
        int u = order[j];
        int d = dist_squared(frame, v, predictions + (j*3));
        if (d < closest_dist) {
            closest = u;
            closest_dist = d;
        }
    }
    return closest;
}


class AxisCompare
{
public:
    AxisCompare(const QuantisedFrame & frame, int dim)
        : m_frame(frame), m_dim(dim) {}

    bool operator () (int i, int j) const {
        for (int k = 0; k < 3; k++) {
            int d = (m_dim + k) % 3;
            unsigned int a = m_frame.quantised_frame[i*3 + d];
            unsigned int b = m_frame.quantised_frame[j*3 + d];
            if (a != b)
                return a < b;
        }
        return false;
    }

    const QuantisedFrame & m_frame;
    int m_dim;
};


Graph * spanning_tree(const QuantisedFrame & frame, int & root,
                      gumhold_predictor * pred)
{
    int atoms = frame.natoms();

    // We process atom order[i] in the ith turn
    vector<int> order(atoms);

    // For now just assign the order as listed. Gumhold et al did this by
    // ordering either by x, y or z coord.
    for (int i = 0; i < atoms; i++)
        order[i] = i;

    // TODO make dim choosable
    int dim = 0;
    AxisCompare cmp(frame, dim);
    sort(order.begin(), order.end(), cmp);
    root = order[0];

    unsigned int predictions[atoms*3];
    pred(&frame, -1, -1, predictions);

    Graph * tree = new Graph(&frame, atoms);

    for (int i = 1; i < atoms; i++) {
        int v = find_closest_seen_atom(frame, order, predictions, i);
        int u = order[i];
        tree->addEdge(v, u);
        pred(&frame, u, v, predictions + (i*3));
    }

    return tree;
}
