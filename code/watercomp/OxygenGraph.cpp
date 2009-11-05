#include "OxygenGraph.h"

#include "ANN/ANN.h"

#include <algorithm>
#include <cassert>
#include <queue>

#define RADIUS_SEARCH 3.0
#define RADIUS_ERROR  0.2

using namespace std;


OxygenGraph::OxygenGraph(const QuantisedFrame & qframe,
                         const vector<WaterMolecule> & waters)
    : m_predictor(qframe), m_qframe(qframe), m_waters(waters),
      nClusters(0), nConstant(0), nHydrogen(0)
{
}


void OxygenGraph::writeout(SerialiseEncoder & enc)
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
    ANNdist radius = RADIUS_SEARCH * RADIUS_SEARCH;
    ANNidxArray result = new ANNidx[oxygen_size];
    ANNdistArray dists = new ANNdist[oxygen_size];

    // Create a graph where each oxygen is a vertex, and it is connected to
    // roughly all other oxygens within RADIUS_SEARCH angstroms.
    for(int i = 0; i < oxygen_size; i++)
    {
        int O_idx = m_waters[i].OH2_index;
        int O_idx_offset = 3 * O_idx;
        for (int d = 0; d < 3; d++)
            query[d] = frame.atom_data[O_idx_offset + d];

        int neigh = kd_tree->annkFRSearch(query, radius, oxygen_size,
                                          result, dists, RADIUS_ERROR);

        for (int j = 0; j < neigh; j++)
            if (result[j] != i)
                graph->addEdge(i, result[j]);
    }

    // Clean up
    annDeallocPts(oxygen_points);
    annDeallocPt(query);
    delete [] result;
    delete [] dists;
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
    unsigned int error = 0;
    for (int d = 0; d < 3; d++)
        error += calc_delta(pred.O[d],  m_qframe.at(O,  d));
    return error;
}


// Creates a spanning tree of the component v is in. Created using a process
// similiar to dijkstra. An edge is added from the root to v.
bool OxygenGraph::create_component(Graph * graph, Graph * tree, int v,
                                   int root) const
{
    int * prediction = (int *) tree->data;

    if (prediction[v] != -1)
        return false;

    int nVerticies = graph->nVerticies;
    int parent[nVerticies];
    int smallest_error[nVerticies];
    fill(smallest_error, smallest_error + graph->nVerticies, INT_MAX);

    priority_queue< pair<int, pair<int, int> > > q; // <-err, pred, v>
    q.push(make_pair(0, make_pair(0, v)));
    parent[v] = root;

    while (!q.empty()) {
        int p =  q.top().second.first;
        int v =  q.top().second.second;
        q.pop();

        // Already processed this edge
        if (prediction[v] != -1)
            continue;

        // If we get to this point, then p is the best prediction we have
        // found for v. This will also be connected to root
        prediction[v] = p;
        if (v != parent[v])
            tree->addEdge(parent[v], v);

        WaterPredictor::Prediction preds[3];
        preds[0] = m_predictor.predict_constant(m_waters[v]);
        preds[1] = m_predictor.predict_along_h1(m_waters[v]);
        preds[2] = m_predictor.predict_along_h2(m_waters[v]);

        for (size_t i = 0; i < graph->adjacent[v].size(); i++) {
            int u = graph->adjacent[v][i];

            // Already processed this edge
            if (prediction[u] != -1)
                continue;

            // Find the best prediction from v to u
            int error = INT_MAX;
            int cur_prediction = -1;
            for (int j = 0; j < 3; j++) {
                int e = prediction_error(u, preds[j]);
                if (e < error) {
                    cur_prediction = j;
                    error = e;
                }
            }

            // This prediction is worse than our current best for u
            if (error > smallest_error[u])
                continue;

            // Update and push
            parent[u] = v;
            smallest_error[u] = error;
            q.push(make_pair(-error, make_pair(cur_prediction, u)));
        }
    }

    return true;
}

Graph * OxygenGraph::create_spanning_tree(Graph * graph, int & root)
{
    int nWaters = m_waters.size();
    int * prediction = new int[nWaters];
    Graph * tree = new Graph(prediction, nWaters);

    fill(prediction, prediction + nWaters, -1);

    nClusters = 1;
    root = 0;
    create_component(graph, tree, root, root);

    for (int v = 1; v < nWaters; v++)
        if (create_component(graph, tree, v, root))
            nClusters++;

    for (int i = 0; i < nWaters; i++) {
        if (prediction[i] == 0)
            nConstant++;
        else
            nHydrogen++;
    }

    return tree;
}


//
// I/O
//

void OxygenGraph::serialise(Graph * tree, int root,
                            SerialiseEncoder & enc) const
{
    int * predictions = (int *) tree->data;
    enc.perm->reset();

    int nWaters = m_waters.size();
    int count = 0;

    vector<int> parent(nWaters);
    parent[root] = -1;

    queue<int> q;
    q.push(root);

    enc.tree_encoder.encode_int(predictions[root]);

    while(!q.empty()) {
        int v = q.front();
        q.pop();
        count++;

        // Get prediction
        WaterMolecule parent_mol(INT_MAX, INT_MAX, INT_MAX);
        if (parent[v] != -1)
            parent_mol = m_waters[parent[v]];
        WaterPredictor::Prediction pred;
        if (predictions[v] == 0)
            pred = m_predictor.predict_constant(parent_mol);
        else if (predictions[v] == 1)
            pred = m_predictor.predict_along_h1(parent_mol);
        else if (predictions[v] == 2)
            pred = m_predictor.predict_along_h2(parent_mol);
        else
            assert(false);

        // Calculate errors
        int O  = m_waters[v].OH2_index;
        int H1 = m_waters[v].H1_index;
        int H2 = m_waters[v].H2_index;
        int error[3][3];
        for (int d = 0; d < 3; d++) {
            unsigned int o_pos = m_qframe.at(O, d);
            error[0][d] = (int)o_pos - pred.O[d];
            error[1][d] = (int)m_qframe.at(H1, d) - o_pos;
            error[2][d] = (int)m_qframe.at(H2, d) - o_pos;
        }

        // Write values to arithmetic encoder
        enc.perm->next_index(v);
        for (int i = 0; i < 3; i++)
            for (int d = 0; d < 3; d++)
                enc.err_encoder.encode_int(error[i][d]);

        // Write predictors for children to encoder and add them to the queue
        for (size_t i = 0; i < tree->adjacent[v].size(); i++) {
            int u = tree->adjacent[v][i];
            int p = predictions[u];

            assert(0 <= u && u < nWaters);
            assert(0 <= p && p < 3);

            enc.tree_encoder.encode_int(p);

            parent[u] = v;
            q.push(u);
        }
        // indicate end of children
        enc.tree_encoder.encode_int(3);
    }

    assert(count == nWaters);
}


void OxygenGraph::readin(SerialiseDecoder & dec,
                         const vector<WaterMolecule> & waters,
                         QuantisedFrame & qframe)
{
    WaterPredictor predictor(qframe);
    dec.perm->reset();

    int nWaters = waters.size();
    int count = 0;

    int root_predictor = dec.tree_decoder.decode_int();

    queue< pair<int, int> > q;
    q.push(make_pair(-1, root_predictor));

    while(!q.empty()) {
        int parent     = q.front().first;
        int prediction = q.front().second;
        q.pop();
        count++;

        // Read in values from arithmetic decoder
        int index = dec.perm->next_index();
        int error[3][3];
        for (int i = 0; i < 3; i++)
            for (int d = 0; d < 3; d++)
                error[i][d] = dec.err_decoder.decode_int();

        assert(0 <= index && index < nWaters);

        // Get predictors for all the children and add them to the queue
        int p;
        while ((p = dec.tree_decoder.decode_int()) != 3) {
            assert(0 <= p && p < 3);
            q.push(make_pair(index, p));
        }

        // Get prediction
        WaterMolecule parent_mol(INT_MAX, INT_MAX, INT_MAX);
        if (parent != -1)
            parent_mol = waters[parent];
        WaterPredictor::Prediction pred;
        if (prediction == 0)
            pred = predictor.predict_constant(parent_mol);
        else if (prediction == 1)
            pred = predictor.predict_along_h1(parent_mol);
        else if (prediction == 2)
            pred = predictor.predict_along_h2(parent_mol);

        // Update qframe
        int O  = waters[index].OH2_index;
        int H1 = waters[index].H1_index;
        int H2 = waters[index].H2_index;
        for (int d = 0; d < 3; d++) {
            unsigned int o_pos = pred.O[d]  + error[0][d];
            qframe.at(O,  d) = o_pos;
            qframe.at(H1, d) = o_pos + error[1][d];
            qframe.at(H2, d) = o_pos + error[2][d];
        }
    }

    assert(count == nWaters);
}
