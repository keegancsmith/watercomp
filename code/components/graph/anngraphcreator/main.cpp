#include "DCDReader.h"
#include "PDBReader.h"
#include "AtomInformation.h"
#include "Frame.h"
#include "FrameSplitter.h"
#include "WaterMolecule.h"
#include "ANNGraphCreator.h"
#include "GridGraphCreator.h"
#include <cstdio>
#include <vector>
#include <map>

using namespace std;

map<int, int> components;
int next_component = 0;
map< unsigned int, std::vector<unsigned int> > graph;
map< unsigned int, unsigned int> sizes;

void dfs(int current, int component)
{
//     printf("Vertex %d is %d\n", current, component);
//     printf("Neighbours: %d\n", graph[current].size());
    
    components[current] = component;
    
    sizes[component] += 1;
    
    for(int i = 0; i < graph[current].size(); ++i)
        if(components.find(graph[current][i]) == components.end())
            dfs(graph[current][i], component);
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        printf("Usage ./driver <PDB_file> <DCD_file>\n");
        return 0;
    }
    
    vector<AtomInformation> pdb = PDBReader::load_pdbfile(argv[1]);
    
//     for(int i = 0; i < pdb.size(); ++i)
//         if(pdb[i].is_water())
//             printf("%u: %s %s %u %s\n", pdb[i].index, pdb[i].atom_name.c_str(), pdb[i].residue_name.c_str(), pdb[i].residue_sequence, pdb[i].seg_id.c_str());

    DCDReader dcdreader(argv[2]);
    float* atoms = new float[3*dcdreader.natoms()];
    
    for(int i = 0 ; i < dcdreader.nframes(); ++i)
    {
        components.clear();
        next_component = 0;
        graph.clear();
        sizes.clear();
        
        dcdreader.next_frame(atoms);
        Frame frame(atoms, dcdreader.natoms());
        QuantisedFrame qframe(frame, 8, 8, 8);
        
        vector<WaterMolecule> waters;
        vector<unsigned int> others;
        FrameSplitter::split_frame(pdb, waters, others);
        
//         graph = ANNGraphCreator::create_graph(waters, frame);
        graph = ANNGraphCreator::create_graph(waters, frame);
        
        printf("%d\n", graph.size());
        
        
        for(int i = 0; i < waters.size(); ++i)
        {
            if(components.find(waters[i].OH2_index) == components.end())
            {
                dfs(waters[i].OH2_index, next_component++);
            }
        }
        
        printf("Clusters: %d\n", next_component);
        
        map<unsigned int, unsigned int> inverse_sizes;
        
        for(map<unsigned int, unsigned int>::iterator it = sizes.begin(); it != sizes.end(); it++)
            inverse_sizes[it->second] += 1;
        
        for(map<unsigned int, unsigned int>::iterator it = inverse_sizes.begin(); it != inverse_sizes.end(); it++)
            printf("%d components have size %d\n", it->second, it->first);
    }
    
    delete[] atoms;
}
