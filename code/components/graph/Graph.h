#pragma once

#include <vector>

class Graph {
public:
    Graph(void * vertexData, int nVerticies);
    ~Graph();

    void addEdge(int from, int to);

    int nVerticies;
    std::vector<int> * adjacent;
    void * data;
};
