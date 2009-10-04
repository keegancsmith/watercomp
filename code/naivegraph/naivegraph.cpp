#include <iostream>

#include "arithmetic/ArithmeticEncoder.h"
#include "graph/GraphCreator.h"
#include "graph/SpanningTree.h"
#include "graph/TreeSerialiser.h"
#include "pdbio/DCDReader.h"
#include "pdbio/PDBReader.h"
#include "quantiser/QuantisedFrame.h"
#include "splitter/FrameSplitter.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

#define QUANTISATION 4

bool compress(const string & dcdpath)
{
    // Load up PDB
    string pdbpath = dcdpath.substr(0, dcdpath.size() - 3) + "pdb";
    vector<AtomInformation> pdb_info = PDBReader::load_pdbfile(pdbpath);

    // Start reading actual DCD
    DCDReader reader;
    if (!reader.open_file(dcdpath.c_str()))
        return false;

    // Output file
    ArithmeticEncoder ae;
    string outpath = dcdpath.substr(0, dcdpath.size() - 3) + "cmp";
    FILE *out = fopen(outpath.c_str(), "w");
    ae.start_encode(out);

    // TODO need to write a header

    // Do naive prediction on each frame
    Frame frame(reader.natoms());
    int frame_num = 0;
    while(reader.next_frame(frame)) {
        cout << frame_num++ << endl;
        QuantisedFrame qframe(frame, QUANTISATION, QUANTISATION, QUANTISATION);
        
        int root;
        Graph * fully_connected_graph = create_graph(&qframe);
        Graph * tree = spanning_tree(fully_connected_graph, root);

        serialise_tree(ae, tree, root);

        delete tree;
        delete fully_connected_graph;
    }

    return true;
}


int main(int main, char ** argv)
{
    return compress(argv[1]) ? 0 : 1;
}
