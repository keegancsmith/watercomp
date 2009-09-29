#include "Frame.h"

#include <cassert>
#include <algorithm>

Frame::Frame(size_t b)
{
    atom_data.reserve(b*3);
}


Frame::Frame(const float * a, size_t b)
{
    copy(a, a + (b * 3), atom_data.begin());
}


size_t Frame::natoms() const
{
    assert(atom_data.size() % 3 == 0);
    return atom_data.size() / 3;
}
