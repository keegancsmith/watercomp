#include "DCDReader.h"

#include <cassert>

DCDReader::DCDReader(const char * dcd_path) {
    m_dcdhandle = (dcdhandle*) open_dcd_read(dcd_path, "dcd", &m_natoms);
}

void DCDReader::next_frame(float * frame) {
    molfile_timestep_t timestep;
    timestep.coords = frame;
    int worked = read_next_timestep(m_dcdhandle, m_natoms, &timestep);
}
