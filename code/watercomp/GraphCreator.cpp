#include "GraphCreator.h"

#include "ANN/ANN.h"

#define RADIUS_SEARCH 3.0
#define RADIUS_ERROR  0.2

Graph * create_oxygen_graph(const QuantisedFrame & qframe,
                            const std::vector<WaterMolecule> waters)
{
    Frame frame = qframe.toFrame();
    Graph * graph = new Graph(&qframe, waters.size());

    // Create the input for the kdtree. This will a kdtree over the
    // unquantised oxygen positions.
    int oxygen_size = waters.size();
    ANNpointArray oxygen_points = annAllocPts(oxygen_size, 3);
    for(int i = 0; i < oxygen_size; i++) {
        int idx_offset = 3 * waters[i].OH2_index;
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
        int O_idx = waters[i].OH2_index;
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
