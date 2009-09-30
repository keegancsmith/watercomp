#pragma once
#include <map>
#include <vector>
#include <set>
#include <cmath>
#include <cassert>
#include <utility>
#include "WaterMolecule.h"
#include "Frame.h"

class GridGraphCreator
{
    public:
        
        static std::map< unsigned int, std::vector<unsigned int> > create_graph(std::vector<WaterMolecule>& waters, Frame& frame)
        {
            /// A grid maintaining a 1 angstrom cubed dimensions
            std::map< std::pair< std::pair< int, int>, int>, std::vector<int> > grid;
            std::map< unsigned int, std::vector<unsigned int> > graph;
            
            for(size_t i = 0; i < waters.size(); ++i)
            {
                int index = waters[i].OH2_index;
                int x = frame.atom_data[3*index];
                int y = frame.atom_data[3*index+1];
                int z = frame.atom_data[3*index+2];
                
                std::pair< std::pair<int, int>, int> coord = std::make_pair(std::make_pair(x, y), z);
                grid[coord].push_back(i);
            }
            
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
                    
                    assert(magnitude >= 0);
                    
                    magnitude = sqrt(magnitude);
                    
                    float direction[3];
                    for(int k = 0; k < 3; ++k)
                        direction[k] = displacement[k]/magnitude;
                    
                    float predicted[3];
                    for(int k = 0; k < 3; ++k)
                        predicted[k] = O_position[k] + direction[k]*2.95;
                    
                    std::vector<int> search_indices;
                    
                    /// Note increasing the following parameters impacts on running time and quality of results.
                    /// Perhaps we need to add in metric to take in closeness of angle. Something like
                    /// Penalty = 100*|Cross Product|
                    /// However we may not want to do this.
                    
                    /// Maximum squared distance allowed
                    float max_dist = 0.2; 
                    int neighbour = -1;
                    
                    for(int x = predicted[0]-1; x <= predicted[0]+1; ++x)
                        for(int y = predicted[1]-1; y <= predicted[1]+1; ++y)
                            for(int z = predicted[2]-1; z <= predicted[2]+1; ++z)
                            {
                                std::pair< std::pair<int, int>, int> coord = std::make_pair(std::make_pair(x, y), z);
                                
                                if(grid.find(coord) == grid.end())
                                    continue;
                                
                                std::vector<int>& water_array = grid[coord];
                                
                                for(int water = 0; water < water_array.size(); ++water)
                                {
                                    
                                    int index = waters[water_array[water]].OH2_index;
                                    
                                    if(index == i)
                                        continue;
                                    
                                    float dx = predicted[0] - frame.atom_data[3*index];
                                    float dy = predicted[1] - frame.atom_data[3*index + 1];
                                    float dz = predicted[2] - frame.atom_data[3*index + 2];
                                    
                                    float dist = dx*dx + dy*dy + dz*dz;
                                    
                                    if(dist < max_dist)
                                    {
                                        max_dist = dist;
                                        neighbour = index;
                                    }
                                }
                            }
                                
                    if(neighbour != -1)
                    {
                        graph[O_index].push_back(neighbour);
                        graph[neighbour].push_back(O_index);
                    }
                }
            }
            
            return graph;
        }
};
