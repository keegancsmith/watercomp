#include "Frame.h"

#include <cassert>
#include <algorithm>

Frame::Frame(size_t b)
{
    atom_data.resize(b*3);
}


Frame::Frame(const float * a, size_t b)
{
    printf("Dddddddddddddddddddddddddddddddddd %u\n", b); 
    atom_data.resize(b*3);
    copy(a, a + (b * 3), atom_data.begin());
}


size_t Frame::natoms() const
{
    assert(atom_data.size() % 3 == 0);
    return atom_data.size() / 3;
}
