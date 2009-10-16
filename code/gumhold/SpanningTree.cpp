#include "SpanningTree.h"

#include <algorithm>
#include <queue>
#include <cassert>
#include <climits>
#include <cmath>
#include <vector>

using namespace std;


int dist_squared(const QuantisedFrame & frame, int a, unsigned int * p)
{
    int d = 0;
    for (int i = 0; i < 3; i++) {
        int x = frame.quantised_frame[a*3 + i];
        int y = p[i];
        int delta = x - y;
        d += (delta * delta);
    }
    return d;
}


int dist_squared2(const QuantisedFrame & frame, int a, int b)
{
    int d = 0;
    for (int i = 0; i < 3; i++) {
        int x = frame.quantised_frame[a*3 + i];
        int y = frame.quantised_frame[b*3 + i];
        int delta = x - y;
        d += (delta * delta);
    }
    return d;
}


// Returns the closest atom to order[i] such that the atom is in order[k]
// where k < i
int find_closest_seen_atom(const QuantisedFrame & frame,
                           const vector<int> & order,
                           unsigned int * predictions,
                           int i)
{
    int v = order[i];
    int closest = order[0];
    int closest_dist = dist_squared(frame, v, predictions);
    for (int j = 1; j < i; j++) {
        int u = order[j];
        int d = dist_squared(frame, v, predictions + (j*3));
        if (d < closest_dist) {
            closest = u;
            closest_dist = d;
        }
    }
    return closest;
}


class AxisCompare
{
public:
    AxisCompare(const QuantisedFrame & frame, int dim)
        : m_frame(frame), m_dim(dim) {}

    bool operator () (int i, int j) const {
        for (int k = 0; k < 3; k++) {
            int d = (m_dim + k) % 3;
            unsigned int a = m_frame.quantised_frame[i*3 + d];
            unsigned int b = m_frame.quantised_frame[j*3 + d];
            if (a != b)
                return a < b;
        }
        return false;
    }

private:
    const QuantisedFrame & m_frame;
    int m_dim;
};


// A grid which assumes points are evenly distributed
class Grid
{
public:
    // Creates a grid based on the bounding box of a quantised frame and the
    // number of atoms in the frame.
    Grid(const QuantisedFrame & frame, int dim) : m_frame(frame) {
        // Calculate the size of the bounding box
        unsigned int quants[3] = { frame.m_xquant, frame.m_yquant,
                                  frame.m_zquant };
        for (int i = 0; i < 3; i++)
            quants[i] = 1 << quants[i];

        // Calculate the size of a cube
        long double area = 1;
        for (int i = 0; i < 3; i++)
            area *= quants[i];
        long double cube_area = area / frame.natoms();
        long double cube_length = powl(cube_area, 1/3.0); // cube root

        // We are dealing with integers, so take the ceiling
        m_length = (unsigned int) ceill(cube_length);
        for (int i = 0; i < 3; i++)
            m_cells[i] = (quants[i] + m_length - 1) / m_length; // ceil

        m_grid.resize(m_cells[0]*m_cells[1]*m_cells[2]);

        int p = 0;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    int delta[3] = {x, y, z};

                    // We dont want to go forwards in the dim dimensions
                    if (delta[dim] == 1)
                        continue;
                    // We want to move
                    if (delta[0] == delta[1] && delta[1] == delta[2]
                        && delta[2] == 0)
                        continue;

                    for (int i = 0; i < 3; i++)
                        m_delta[p][i] = delta[i];
                    p++;
                }
            }
        }
        assert(p == 8 + 9);
    }


    void add_point(int atom_idx) {
        unsigned int points[3];
        for (int i = 0; i < 3; i++)
            points[i] = m_frame.quantised_frame[atom_idx*3 + i];
        unsigned int idx = point_to_idx(points);
        m_grid[idx].push_back(atom_idx);
    }

    int find_closest(int atom_idx) {
        unsigned int points[3];
        for (int i = 0; i < 3; i++)
            points[i] = m_frame.quantised_frame[atom_idx*3 + i];
        unsigned int idx = point_to_idx(points);

        int closest = -1;
        int closest_dist = INT_MAX;
        int closest_depth = 0;

        vector<bool> seen(m_grid.size());
        queue< pair<unsigned int, int> > q;
        q.push(make_pair(idx, 0));
        seen[idx] = true;

        while (!q.empty()) {
            idx = q.front().first;
            int depth = q.front().second;
            q.pop();

            if (depth != closest_depth) {
                int d = depth - closest_depth;
                assert(d == 1 || d == 2);
                // TODO this is slower so commented out, but might give better
                // results.
                // if (closest != -1 && d == 2)
                //     return closest;
                if (closest == -1)
                    closest_depth = depth;
                else
                    return closest; // TODO remove if above todo is
                                    // uncommented
            }

            for (size_t i = 0; i < m_grid[idx].size(); i++) {
                int u = m_grid[idx][i];
                int d = dist_squared2(m_frame, atom_idx, u);
                if (d < closest_dist) {
                    closest = u;
                    closest_dist = d;
                }
            }

            unsigned int cell[3];
            idx_to_cell(idx, cell);

            for (int i = 0; i < 8 + 9; i++) {
                bool inside = true;
                unsigned int c[3];
                for (int j = 0; j < 3; j++) {
                    c[j] = m_delta[i][j] + cell[j];
                    if (c[j] >= m_cells[j])
                        inside = false;
                }
                if (!inside)
                    continue;

                unsigned int idx2 = cell_to_idx(c);
                if (seen[idx2])
                    continue;
                seen[idx2] = true;
                q.push(make_pair(idx2, depth+1));
            }
        }

        assert(false);
        return -1;
    }

private:

    unsigned int cell_to_idx(const unsigned int * c) {
        unsigned int idx = m_cells[2]*(m_cells[1]*c[0] + c[1]) + c[2];
        assert(idx < m_grid.size());
        return idx;
    }

    void idx_to_cell(unsigned int idx, unsigned int * c) {
        assert(idx < m_grid.size());
        c[2] = idx % m_cells[2];
        c[1] = (idx / m_cells[2]) % m_cells[1];
        c[0] = (idx / m_cells[2]) / m_cells[1];
    }

    unsigned int point_to_idx(const unsigned int * points) {
        unsigned int c[3];
        for (int i = 0; i < 3; i++) {
            c[i] = points[i] / m_length;
            assert(c[i] < m_cells[i]);
        }
        return cell_to_idx(c);
    }


    const QuantisedFrame & m_frame;
    vector< vector<int> > m_grid;
    int m_delta[8+9][3];
    unsigned int m_length;
    unsigned int m_cells[3];
};


Graph * spanning_tree(const QuantisedFrame & frame, int & root,
                      gumhold_predictor * pred)
{
    int atoms = frame.natoms();

    // We process atom order[i] in the ith turn
    vector<int> order(atoms);

    // For now just assign the order as listed. Gumhold et al did this by
    // ordering either by x, y or z coord.
    for (int i = 0; i < atoms; i++)
        order[i] = i;

    // TODO make dim choosable
    int dim = 0;
    AxisCompare cmp(frame, dim);
    sort(order.begin(), order.end(), cmp);
    root = order[0];

    //unsigned int predictions[atoms*3];
    //pred(&frame, -1, -1, predictions);

    Graph * tree = new Graph(&frame, atoms);
    Grid grid(frame, dim);
    grid.add_point(root);

    for (int i = 1; i < atoms; i++) {
        //int v = find_closest_seen_atom(frame, order, predictions, i);
        int u = order[i];
        int v = grid.find_closest(u);
        grid.add_point(u);
        tree->addEdge(v, u);
        //pred(&frame, u, v, predictions + (i*3));
    }

    return tree;
}
