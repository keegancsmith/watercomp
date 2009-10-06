#include "NaiveWriter.h"
#include "arithmetic/ByteEncoder.h"
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
    m_encoder.start_encode(m_fout);
    ByteEncoder enc(&m_encoder);

    // File header
    int header_int[4] = { atoms, frames, ISTART, NSAVC };
    enc.encode(header_int, sizeof(int), 4);
    enc.encode(&DELTA, sizeof(double), 1);
}


void NaiveWriter::next_frame(const QuantisedFrame& qframe)
{
    ByteEncoder enc(&m_encoder);

    // Frame header
    unsigned int header_quant[3] = {
        qframe.m_xquant, qframe.m_yquant, qframe.m_zquant
    };
    enc.encode(header_quant, sizeof(unsigned int), 3);
    enc.encode(qframe.min_coord, sizeof(float), 3);
    enc.encode(qframe.max_coord, sizeof(float), 3);

    // Create spanning tree of atoms
    int root;
    Graph * fully_connected_graph = create_graph(&qframe);
    Graph * tree = spanning_tree(fully_connected_graph, root);

    // Output the spanning tree
    serialise_tree(m_encoder, tree, root);

    // Cleanup
    delete tree;
    delete fully_connected_graph;
}


void NaiveWriter::end()
{
    m_encoder.end_encode();
}
