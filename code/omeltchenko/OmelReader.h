#pragma once
#include <cstdio>
#include <queue>
#include <vector>
#include "FrameReader.h"
#include "OmelDecoder.h"
#include "quantiser/QuantisedFrame.h"

class OmelReader : public FrameReader
{
    public:
        /* Constructs a Omeltchenko reader object to read from the specified file. */
        OmelReader(FILE* input_file) : in_file(input_file), frames_left(0), atoms(0), istart(0), nsavc(1), delta(1.0) {};
        
        /* Accessor for number of atoms */
        int get_atoms() { return atoms; };
        
        /* Accessor for frames written */
        int get_frames() { return frames_left; };
        
        /* Accessors for dcd header data */
        int get_istart() { return istart; };
        int get_nsavc() { return nsavc; };
        double get_delta() { return delta; };
        
        
        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start();
        
        /* Reads a frame */
        bool next_frame(QuantisedFrame& qframe);
        
        /* Ends the reading process. */
        void end();
        
    private:
        /* File required for input */
        FILE* in_file;
        
        /* Number of frames */
        int frames_left;
        
        /* Number of atoms in each frame */
        int atoms;
        
        /* Data from the DCD file header */
        int istart;
        int nsavc;
        double delta;
};
