#pragma once

#include <vector>
#include "WaterMolecule.h"
#include "../quantiser/QuantisedFrame.h"
#include "../pdbio/AtomInformation.h"

void split_frame(const std::vector<AtomInformation> & pdb,
                       std::vector<WaterMolecule>& water_molecules,
                       std::vector<unsigned int>& other_atoms);
                       
                       