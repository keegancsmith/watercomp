#pragma once
#include <cstdio>
#include <queue>
#include <vector>
#include "OmelEncoder.h"
#include "FrameWriter.h"
#include "quantiser/QuantisedFrame.h"
#include "SortedQuantisedFrame.h"


class OmelWriter : public FrameWriter
{
    public:
        /* Constructs a Omeltchenko reader object to read from the specified file. */
        OmelWriter(FILE* output_file) : out_file(output_file) {};
        
        ~OmelWriter();
        
        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start(int atoms, int frames, int ISTART = 0, int NSAVC = 1, double DELTA = 0.0);
        
        /* Writes a frame */
        void next_frame(const QuantisedFrame & qframe);
        
        /* Ends the reading process. */
        void end();
        
    private:
        /* File for saving */
        FILE* out_file;
};
