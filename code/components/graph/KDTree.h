#pragma once

#include <vector>
#include <queue>
#include <algorithm>

class KDTree
{
public:
    KDTree(std::vector<unsigned int> * p, std::vector<int> * i,
           int d=0, int l=0, int u=-1);
    ~KDTree();

    template<class Predicate>
    void iterate_closest(int x, int y, int z, Predicate & pred);

    unsigned int m_min_box[3];
    unsigned int m_max_box[3];

private:
    void find_box();
    void create_children();

    std::vector<unsigned int> * m_points;
    std::vector<int> * m_index;
    int m_lower, m_upper;
    int m_dimension;
    KDTree * m_left;
    KDTree * m_right;
};


// Below has to be here otherwise linker errors galore
class SearchCmp
{
public:
    SearchCmp(int x, int y, int z) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }

    // manhattan dist
    int dist_from_v(const KDTree * node) const {
        int d = 0;
        for (int i = 0; i < 3; i++) {
            int a = v[i] - node->m_min_box[i];
            int b = v[i] - node->m_max_box[i];
            if (a < 0)
                a *= -1;
            if (b < 0)
                b *= -1;
            d += std::min(a, b);
        }
        return d;
    }

    bool operator ()(const KDTree*a, const KDTree*b) const {
        int a_dist = dist_from_v(a);
        int b_dist = dist_from_v(b);
        // reverse of what you expect cause want minheap
        return b_dist < a_dist;
    }

    int v[3];
};


template<class Predicate>
void KDTree::iterate_closest(int x, int y, int z, Predicate & pred)
{
    SearchCmp cmp(x, y, z);
    std::priority_queue<KDTree*, std::vector<KDTree*>, SearchCmp> q(cmp);

    q.push(this);

    while (!q.empty()) {
        KDTree * node = q.top();
        q.pop();

        // If we are at a leaf check if we can stop searching
        if (node->m_upper - node->m_lower == 1) {
            bool finish = pred(m_index->at(node->m_lower));
            if (finish)
                return;
            else
                continue;
        }

        // Add children
        q.push(node->m_left);
        q.push(node->m_right);
    }
}
