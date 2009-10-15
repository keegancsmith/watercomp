#include "ANNGraphCreator.h"
#include <map>
#include <vector>
#include <cmath>
#include <cassert>
#include <ANN/ANN.h>
#include "WaterMolecule.h"
#include "Frame.h"

using namespace std;

map< unsigned int, vector<unsigned int> > create_graph(vector<WaterMolecule>& waters, Frame& frame, double tolerance, double predicted_distance)
{
    map< unsigned int, vector<unsigned int> > graph;
    
    int nPts = waters.size();
    ANNpointArray dataPts = annAllocPts(nPts, 3);
    for(int i = 0; i < nPts; ++i)
        for(int j = 0; j < 3; ++j)
            dataPts[i][j] = frame.atom_data[3*waters[i].OH2_index+j];
    
    ANNpoint queryPt = annAllocPt(3);
    ANNidxArray nnIdx = new ANNidx[1];
    ANNdistArray dists = new ANNdist[1];
    ANNkd_tree* kdTree = new ANNkd_tree(dataPts, nPts, 3);
    
    for(size_t i = 0; i < waters.size(); ++i)
    {
        int O_index = waters[i].OH2_index;
        int H_indexes[2] = {waters[i].H1_index, waters[i].H2_index};
        
        for(int j = 0; j < 2; ++j)
        {
            float O_position[3] = {frame.atom_data[3*O_index], frame.atom_data[3*O_index+1], frame.atom_data[3*O_index+2]};
            
            float H_position[3] = {frame.atom_data[3*H_indexes[j]], frame.atom_data[3*H_indexes[j]+1], frame.atom_data[3*H_indexes[j]+2]};
            
            float displacement[3];
            for(int k = 0; k < 3; ++k)
                displacement[k] = H_position[k] - O_position[k];
            
            float magnitude = 0;
            for(int k = 0; k < 3; ++k)
                magnitude += displacement[k]*displacement[k];
            
            assert(magnitude > 0);
            
            magnitude = sqrt(magnitude);
            
            float direction[3];
            for(int k = 0; k < 3; ++k)
                direction[k] = displacement[k]/magnitude;
            
            float predicted[3];
            for(int k = 0; k < 3; ++k)
                predicted[k] = O_position[k] + direction[k]*predicted_distance;
            
            for(int k = 0; k < 3; ++k)
                queryPt[k] = predicted[k];
            
            /// Note increasing the following parameters impacts on running time and quality of results.
            /// Perhaps we need to add in metric to take in closeness of angle. Something like
            /// Penalty = 100*|Cross Product|
            /// However we may not want to do this.
            
            /// Maximum squared distance allowed
            float max_dist = tolerance; 
            int neighbour = -1;
            
            kdTree->annkSearch(queryPt, 1, nnIdx, dists, 0.0);
            
            if(dists[0] <= max_dist)
            {
                graph[O_index].push_back(waters[nnIdx[0]].OH2_index);
                graph[waters[nnIdx[0]].OH2_index].push_back(O_index);
            }
        }
    }
    
    delete [] nnIdx;
    delete [] dists;
    delete kdTree;
    
    return graph;
}
