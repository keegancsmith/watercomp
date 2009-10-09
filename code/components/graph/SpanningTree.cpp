#include "SpanningTree.h"
#include "KDTree.h"

#include <climits>
#include <functional>
#include <queue>
#include <vector>

using std::vector;
using std::queue;


// Find closest unseen atom
class FindAtoms
{
public:
    FindAtoms(const QuantisedFrame * frame, vector<bool> * seen, size_t k)
        : frame(frame), seen(seen), k(k) {}

    bool operator() (int i) {
        if (!seen->at(i))
            atoms_idx.push_back(i);
        return atoms_idx.size() == k;
    }

    const QuantisedFrame * frame;
    vector<bool> * seen;
    size_t k;
    vector<int> atoms_idx;
};

Graph * spanning_tree(const QuantisedFrame & frame, int & root) {
    vector<bool> seen;
    seen.resize(frame.natoms());

    // Create KD-Tree
    vector<unsigned int> points(frame.quantised_frame);
    vector<int> index;
    index.resize(frame.natoms());
    for (size_t i = 0; i < index.size(); i++)
        index[i] = i;
    KDTree search_tree(&points, &index);

    Graph * tree = new Graph(&frame, frame.natoms());

    root = 0;
    seen[root] = true;

    queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        // Find 2 closest unseen atoms
        FindAtoms fa(&frame, &seen, 2);
        search_tree.iterate_closest(frame.quantised_frame[v*3 + 0],
                                    frame.quantised_frame[v*3 + 1],
                                    frame.quantised_frame[v*3 + 2],
                                    fa);

        // Add the unseen atoms to the queue
        for (size_t i = 0; i < fa.atoms_idx.size(); i++) {
            int k = fa.atoms_idx[i];
            seen[k] = true;
            tree->addEdge(v, k);
            q.push(k);
        }
    }

    return tree;
}
