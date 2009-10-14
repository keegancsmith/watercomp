#pragma once
#include <string>
#include <vector>
#include "AtomInformation.h"

std::vector<AtomInformation> load_pdbfile(std::string filename);
