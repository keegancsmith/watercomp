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
    int       n;                // There are n points in S
    int       dim;              // The dimension we are currently splitting by
};

bool is_unit(const segment_t & S) {
    // Note that this actually returns true for the unit segment and 0.
    for (int i = 0; i < DIMENSIONS; i++)
        if ((S.to[i] - S.from[i]) > 1)
            return false;
    return true;
}


vector<coord_t> encode(const vector<point_t> & points, unsigned int bits)
{
    vector<coord_t> data;
    queue<encode_queue_item_t> L;

    assert(sizeof(coord_t) > bits);
    
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

        if (left.points.size() > 0 && !is_unit(left.S))
            L.push(left);
        if (right.points.size() > 0 && !is_unit(right.S))
            L.push(right);
    }

    return data;
}
