#pragma once

#include "AtomInformation.h"

#include <vector>

// Simple container for info relevant to atom information. Does not "own" any
// of its members. So does not free atom_data and atom_information in the
// deconstructor.
class Frame
{
public:
    Frame(float * atom_data, size_t natoms,
          std::vector<AtomInformation> *atom_information);

    float * atom_data;
    size_t natoms;
    std::vector<AtomInformation> * atom_information;
};
