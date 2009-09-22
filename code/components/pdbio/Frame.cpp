#include "Frame.h"

#include <cassert>

Frame::Frame(float * a, size_t b,
             std::vector<AtomInformation> *c) :
    atom_data(a), natoms(b), atom_information(c) {

    assert(atom_information->size() == natoms);
}
