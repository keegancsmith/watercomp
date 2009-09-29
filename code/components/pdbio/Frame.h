#pragma once

#include "AtomInformation.h"

#include <vector>

class Frame
{
public:
    Frame(size_t natoms);
    Frame(const float * atom_data, size_t natoms);

    size_t natoms() const;

    std::vector<float> atom_data;
};
