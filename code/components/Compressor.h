#pragma once

#include "FrameReader.h"
#include "FrameWriter.h"
#include "pdbio/AtomInformation.h"

#include <cstdio>
#include <vector>

class Compressor
{
public:
    virtual FrameReader * frame_reader(FILE * fin) = 0;
    virtual FrameWriter * frame_writer(FILE * fout) = 0;

    virtual bool needs_atom_information() const { return false; }

    std::vector<AtomInformation> m_atom_information;
};


int do_main(Compressor & c, int argc, char ** argv);
