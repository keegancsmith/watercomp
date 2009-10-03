#include "GraphCreator.h"

Graph * create_graph(const QuantisedFrame * frame) {
    Graph * graph = new Graph(frame, frame->natoms());

    for (int i = 0; i < frame->natoms(); i++) {
        graph->adjacent[i].resize(frame->natoms()-1);
        for (int j = 0; j < frame->natoms(); j++)
            if (j != i)
                graph->addEdge(i, j);
    }

    return graph;
}
