#pragma once

#include "FrameReader.h"
#include "FrameWriter.h"

#include <cstdio>

class Compressor
{
public:
    virtual FrameReader * frame_reader(FILE * fin) = 0;
    virtual FrameWriter * frame_writer(FILE * fout) = 0;
};


int do_main(Compressor & c, int argc, char ** argv);
