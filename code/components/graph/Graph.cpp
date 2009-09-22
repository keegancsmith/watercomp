#include "Graph.h"

#include <cassert>

Graph::Graph(void * data, int nVerticies)
    : nVerticies(nVerticies), data(data) {
    adjacent = new std::vector<int>[nVerticies];
}

Graph::~Graph() {
    delete[] adjacent;
}

void Graph::addEdge(int from, int to) {
    assert(0 <= from && from < nVerticies);
    assert(0 <= to && to < nVerticies);

    adjacent[from].push_back(to);
}
