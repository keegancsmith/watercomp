#ifndef _devillers_h_
#define _devillers_h_

#include <vector>

#ifndef DIMENSIONS
#define DIMENSIONS 3
#endif

// needs to be unsigned and sizeof(coord_t) >= BITS
typedef unsigned long coord_t;

struct point_t {
    coord_t coords[DIMENSIONS];
};

// TODO this design is not the most memory efficient. Possibly experiment with
// iterators/unary_functions.

/**
 * @brief Does the devillers and gandoin transformation on the list of points.
 *
 * The transformation gives an ordered list of integers to be compressed.
 *
 * @bits The maximum number of bits each coordinate will be
 */
std::vector<coord_t> encode(const std::vector<point_t> & points, unsigned int bits);

/**
 * @brief Does the devillers and gandoin reverse transformation to get back a
 * list of points.
 *
 * @bits The maximum number of bits each coordinate will be
 */
std::vector<point_t> decode(const std::vector<coord_t> & data, unsigned int bits);


#endif
