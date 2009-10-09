#pragma once

#include <vector>

class KDTree
{
public:
    KDTree(std::vector<unsigned int> * p, std::vector<int> * i,
           int d=0, int l=0, int u=-1);
    ~KDTree();

private:
    void find_box();
    void create_children();

    std::vector<unsigned int> * m_points;
    std::vector<int> * m_index;
    int m_lower, m_upper;
    int m_dimension;
    unsigned int m_min_box[3];
    unsigned int m_max_box[3];
    KDTree * m_left;
    KDTree * m_right;
};
