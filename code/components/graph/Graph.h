#pragma once

#include <vector>

class Graph {
public:
    Graph(const void * vertexData, int nVerticies);
    ~Graph();

    void addEdge(int from, int to);

    int nVerticies;
    std::vector<int> * adjacent;
    const void * data;
};
