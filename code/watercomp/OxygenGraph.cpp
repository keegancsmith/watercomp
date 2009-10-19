#include "OxygenGraph.h"

#include "ANN/ANN.h"

#define RADIUS_SEARCH 3.0
#define RADIUS_ERROR  0.2

using std::vector;


OxygenGraph::OxygenGraph(const QuantisedFrame & qframe,
                         const vector<WaterMolecule> & waters)
    : m_qframe(qframe), m_waters(waters)
{
}


void OxygenGraph::writeout(AdaptiveModelEncoder & enc) const
{
    int root;
    Graph * graph = create_oxygen_graph();
    Graph * tree  = create_spanning_tree(graph, root);
    serialise(tree, root, enc);

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


Graph * OxygenGraph::create_spanning_tree(Graph * graph, int & root) const
{
    return 0;
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
