#include "OxygenGraph.h"

#include "ANN/ANN.h"

#include <algorithm>
#include <queue>

#define RADIUS_SEARCH 3.0
#define RADIUS_ERROR  0.2

using std::vector;


OxygenGraph::OxygenGraph(const QuantisedFrame & qframe,
                         const vector<WaterMolecule> & waters)
    : m_predictor(qframe), m_qframe(qframe), m_waters(waters)
{
}


void OxygenGraph::writeout(AdaptiveModelEncoder & enc) const
{
    int root;
    Graph * graph = create_oxygen_graph();
    Graph * tree  = create_spanning_tree(graph, root);
    int * predictions = (int *) tree->data;
    serialise(tree, root, enc);

    delete [] predictions;
    delete tree;
    delete graph;
}


//
// Graph Creation
//

Graph * OxygenGraph::create_oxygen_graph() const
{
    Frame frame = m_qframe.toFrame();
    Graph * graph = new Graph(0, m_waters.size());

    // Create the input for the kdtree. This will a kdtree over the
    // unquantised oxygen positions.
    int oxygen_size = m_waters.size();
    ANNpointArray oxygen_points = annAllocPts(oxygen_size, 3);
    for(int i = 0; i < oxygen_size; i++) {
        int idx_offset = 3 * m_waters[i].OH2_index;
        for(int d = 0; d < 3; d++)
            oxygen_points[i][d] = frame.atom_data[idx_offset + d];
    }

    // Create the kdtree
    ANNkd_tree* kd_tree = new ANNkd_tree(oxygen_points, oxygen_size, 3);

    // Stuff used per query of the kdtree
    ANNpoint query = annAllocPt(3);
    ANNidxArray result = new ANNidx[oxygen_size];
    ANNdistArray dists = 0; // Not used

    // Create a graph where each oxygen is a vertex, and it is connected to
    // roughly all other oxygens within RADIUS_SEARCH angstroms.
    for(int i = 0; i < oxygen_size; i++)
    {
        int O_idx = m_waters[i].OH2_index;
        int O_idx_offset = 3 * O_idx;
        for (int d = 0; d < 3; d++)
            query[d] = frame.atom_data[O_idx_offset + d];

        int neigh = kd_tree->annkFRSearch(query, RADIUS_SEARCH*RADIUS_SEARCH,
                                          0, result, dists, RADIUS_ERROR);

        for (int j = 0; j < neigh; j++)
            if (result[j] != i)
                graph->addEdge(i, result[j]);
    }

    // Clean up
    annDeallocPts(oxygen_points);
    annDeallocPt(query);
    delete [] result;
    delete kd_tree;

    return graph;
}


unsigned int calc_delta(unsigned int a, unsigned int b)
{
    if (a < b)
        return calc_delta(b, a);
    return a - b;
}


int OxygenGraph::prediction_error(int idx,
                                  WaterPredictor::Prediction & pred) const
{
    int O  = m_waters[idx].OH2_index;
    int H1 = m_waters[idx].H1_index;
    int H2 = m_waters[idx].H2_index;
    unsigned int error = 0;
    for (int d = 0; d < 3; d++) {
        error += calc_delta(pred.O[d],  m_qframe.at(O,  d));
        error += calc_delta(pred.H1[d], m_qframe.at(H1, d));
        error += calc_delta(pred.H2[d], m_qframe.at(H2, d));
    }
    return error;
}


bool OxygenGraph::create_component(Graph * graph, Graph * tree, int v) const
{
    int * prediction = (int *) tree->data;

    if (prediction[v] != -1)
        return false;

    std::queue<int> q;
    q.push(v);
    prediction[v] = 0;

    while (!q.empty()) {
        v = q.front();
        q.pop();

        for (size_t i = 0; i < graph->adjacent[v].size(); i++) {
            int u = graph->adjacent[v][i];

            if (prediction[u] != -1)
                continue;

            WaterPredictor::Prediction preds[3];
            preds[0] = m_predictor.predict_constant(m_waters[u]);
            preds[1] = m_predictor.predict_along_h1(m_waters[u]);
            preds[2] = m_predictor.predict_along_h2(m_waters[u]);

            int error = INT_MAX;
            for (int j = 0; j < 3; j++) {
                int e = prediction_error(u, preds[j]);
                if (e < error)
                    prediction[u] = j;
            }

            tree->addEdge(v, u);
            q.push(u);
        }
    }

    return true;
}

Graph * OxygenGraph::create_spanning_tree(Graph * graph, int & root) const
{
    int nWaters = m_waters.size();
    int * prediction = new int[nWaters];
    Graph * tree = new Graph(prediction, nWaters);

    std::fill(prediction, prediction + nWaters, -1);
    root = 0;
    create_component(graph, tree, root);

    for (int v = 1; v < nWaters; v++)
        if (create_component(graph, tree, v))
            tree->addEdge(root, v);

    return tree;
}


//
// I/O
//

void OxygenGraph::serialise(Graph * tree, int root,
                            AdaptiveModelEncoder & enc) const
{
}


void OxygenGraph::readin(AdaptiveModelDecoder & dec,
                         const vector<WaterMolecule> & waters,
                         QuantisedFrame & qframe)
{
}
