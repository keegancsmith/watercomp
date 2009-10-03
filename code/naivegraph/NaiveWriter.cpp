#include "NaiveWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "graph/GraphCreator.h"
#include "graph/SpanningTree.h"
#include "graph/TreeSerialiser.h"

NaiveWriter::NaiveWriter(FILE * fout)
    : m_fout(fout)
{
}

NaiveWriter::~NaiveWriter()
{
}

void NaiveWriter::start(int atoms, int frames, int ISTART,
                        int NSAVC, double DELTA)
{
    // File header
    int header_int[4] = { atoms, frames, ISTART, NSAVC };
    fwrite(header_int, sizeof(int), 4, m_fout);
    fwrite(&DELTA, sizeof(double), 1, m_fout);
}

void NaiveWriter::next_frame(const QuantisedFrame& qframe)
{
    // Frame header
    unsigned int header_quant[3] = {
        qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
    };
    fwrite(header_quant, sizeof(unsigned int), 3, m_fout);
    fwrite(qframe.min_coord, sizeof(float), 3, m_fout);
    fwrite(qframe.max_coord, sizeof(float), 3, m_fout);

    // Create spanning tree of atoms
    int root;
    Graph * fully_connected_graph = create_graph(&qframe);
    Graph * tree = spanning_tree(fully_connected_graph, root);

    // Output the spanning tree
    ArithmeticEncoder ae;
    ae.start_encode(m_fout);
    serialise_tree(ae, tree, root);
    ae.end_encode();

    // Cleanup
    delete tree;
    delete fully_connected_graph;
}

void NaiveWriter::end()
{
}
