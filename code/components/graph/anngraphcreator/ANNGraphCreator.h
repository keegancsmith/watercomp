#pragma once
#include <map>
#include <vector>
#include "WaterMolecule.h"
#include "Frame.h"


std::map< unsigned int, std::vector<unsigned int> > create_graph(std::vector<WaterMolecule>& waters, Frame& frame);