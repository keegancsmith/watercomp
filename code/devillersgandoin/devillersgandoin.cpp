#include "devillersgandoin.h"

#include <utility>
#include <queue>
#include <cassert>

using std::vector;
using std::pair;
using std::queue;

// 1D is segment and is what the psuedocode from the paper uses. Since this is
// most likely 3D, it should be called cell_t.
struct segment_t {
    coord_t from[DIMENSIONS];
    coord_t to[DIMENSIONS];
};

struct encode_queue_item_t {
    segment_t       S;
    vector<point_t> points;     // all points lie in S
    int             dim;        // The dimension we are currently splitting by
};

struct decode_queue_item_t {
    segment_t S;
    coord_t   n;                // There are n points in S
    int       dim;              // The dimension we are currently splitting by
};

bool is_unit(const segment_t & S) {
    // Note that this actually returns true for the unit segment and 0.
    for (int i = 0; i < DIMENSIONS; i++)
        if ((S.to[i] - S.from[i]) > 1)
            return false;
    return true;
}


vector<coord_t> encode(const vector<point_t> & points,
                       std::vector<int> & permutation,
                       unsigned int bits)
{
    vector<coord_t> data;
    queue<encode_queue_item_t> L;

    assert(sizeof(coord_t)*8 > bits);

    // Initially we look at all points are in [0,2^bits)^DIMENSIONS
    encode_queue_item_t initial;
    initial.points = points;
    initial.dim = 0;
    for (int i = 0; i < DIMENSIONS; i++) {
        initial.S.from[i] = 0;
        initial.S.to[i]   = 1 << bits;
    }

    L.push(initial);

    data.push_back(initial.points.size());

    while (!L.empty()) {
        encode_queue_item_t x = L.front();
        L.pop();
        coord_t mid = (x.S.from[x.dim] + x.S.to[x.dim]) / 2;

        encode_queue_item_t left;
        left.dim = (x.dim + 1) % DIMENSIONS;
        left.S = x.S;
        left.S.to[x.dim] = mid;

        encode_queue_item_t right;
        right.dim = (x.dim + 1) % DIMENSIONS;
        right.S = x.S;
        right.S.from[x.dim] = mid;

        for(size_t i = 0; i < x.points.size(); i++) {
            if (x.points[i].coords[x.dim] < mid)
                left.points.push_back(x.points[i]);
            else
                right.points.push_back(x.points[i]);
        }

        data.push_back(left.points.size());

        if (left.points.size() > 0) {
            if (is_unit(left.S)) {
                for (size_t i = 0; i < left.points.size(); i++)
                    permutation.push_back(left.points[i].index);
            } else {
                L.push(left);
            }
        }
        if (right.points.size() > 0) {
            if (is_unit(right.S)) {
                for (size_t i = 0; i < right.points.size(); i++)
                    permutation.push_back(right.points[i].index);
            } else {
                L.push(right);
            }
        }
    }

    return data;
}

point_t project_1(const segment_t & S) {
    point_t r;
    for (int i = 0; i < DIMENSIONS; i++)
        r.coords[i] = S.from[i];
    return r;
}

vector<point_t> decode(const vector<coord_t> & data, unsigned int bits)
{
    vector<point_t> points;
    queue<decode_queue_item_t> L;
    size_t i = 0;

    assert(sizeof(coord_t)*8 > bits);

    // Initially we look at all points are in [0,2^bits)^DIMENSIONS
    decode_queue_item_t initial;
    initial.n = data[i++];
    initial.dim = 0;
    for (int i = 0; i < DIMENSIONS; i++) {
        initial.S.from[i] = 0;
        initial.S.to[i]   = 1 << bits;
    }

    L.push(initial);

    while(!L.empty()) {
        decode_queue_item_t x = L.front();
        L.pop();
        coord_t mid = (x.S.from[x.dim] + x.S.to[x.dim]) / 2;

        decode_queue_item_t left;
        left.dim = (x.dim + 1) % DIMENSIONS;
        left.S = x.S;
        left.S.to[x.dim] = mid;

        decode_queue_item_t right;
        right.dim = (x.dim + 1) % DIMENSIONS;
        right.S = x.S;
        right.S.from[x.dim] = mid;

        left.n = data[i++];
        right.n = x.n - left.n;

        if (left.n > 0) {
            if (is_unit(left.S)) {
                point_t p = project_1(left.S);
                for (coord_t j = 0; j < left.n; j++)
                    points.push_back(p);
            } else {
                L.push(left);
            }
        }
        if (right.n > 0) {
            if (is_unit(right.S)) {
                point_t p = project_1(right.S);
                for (coord_t j = 0; j < right.n; j++)
                    points.push_back(p);
            } else {
                L.push(right);
            }
        }
    }

    return points;
}
