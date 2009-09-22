#pragma once

#include "../../dcd_loader/dcdplugin.h"

class DCDReader {
public:
    DCDReader(const char * dcd_path);

    /**
     * Reads the next frame into frame.
     *
     * frame has to be an array of size = 3 * natoms().
     * Atom i's j'th coordinate is stored in frame[3*i + j].
     */
    void next_frame(float * frame);

    size_t nframes() const { return m_dcdhandle->nsets; }
    int natoms() const  { return m_natoms; }

private:
    dcdhandle * m_dcdhandle;
    int m_natoms;
};
