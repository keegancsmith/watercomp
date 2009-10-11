#include "KDTree.h"

#include <algorithm>
#include <cassert>
#include <climits>

using namespace std;


KDTree::KDTree(vector<unsigned int> * p, vector<int> * i, int d, int l, int u)
    : m_points(p), m_index(i), m_lower(l), m_upper(u), m_dimension(d),
      m_left(0), m_right(0)
{
    if (m_upper == -1)
        m_upper = m_index->size();
    assert(m_index->size() * 3 == m_points->size());
    assert(m_points->size() >= 1);
    assert(m_upper - m_lower > 0);

    find_box();
    create_children();
}


void KDTree::find_box()
{
    for (int i = 0; i < 3; i++) {
        m_min_box[i] = INT_MAX;
        m_max_box[i] = 0;
    }
    for (int i = m_lower; i < m_upper; i++) {
        for (int j = 0; j < 3; j++) {
            m_min_box[j] = min(m_min_box[j], m_points->at(i*3 + j));
            m_max_box[j] = max(m_max_box[j], m_points->at(i*3 + j));
        }
    }
}


KDTree::~KDTree()
{
    if (m_left)
        delete m_left;
    if (m_right)
        delete m_right;
    m_left = m_right = 0;
}


void KDTree::create_children()
{
    // Only one child left
    // TODO it could be possible to have more than one atom at the same coord
    if (m_lower == m_upper - 1)
        return;

    // Pivot value is chosen using the bounding box, so make sure all the
    // points are not axis aligned in the new dimension.
    int d = m_dimension;
    do {
        d = (d + 1) % 3;
    } while (m_max_box[d] == m_min_box[d]);

    unsigned int pivot_val = (m_max_box[d] + m_min_box[d] + 1) / 2;
    int pivot_idx = m_lower;

    // Partition points so if a point index is < pivot_idx then point[d] <
    // pivot_val
    for (int i = m_lower; i < m_upper; i++) {
        if (m_points->at(3*i + d) < pivot_val) {
            for (int j = 0; j < 3; j++)
                swap(m_points->at(3*i + j), m_points->at(3*pivot_idx + j));
            swap(m_index->at(i), m_index->at(pivot_idx));
            pivot_idx++;
        }
    }

    assert(pivot_idx != m_lower);
    assert(pivot_idx != m_upper);

    m_left  = new KDTree(m_points, m_index, d, m_lower, pivot_idx);
    m_right = new KDTree(m_points, m_index, d, pivot_idx, m_upper);
}