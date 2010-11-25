#pragma once
#include <vector>
#include <deque>
#include <cstdio>
#include "FrameReader.h"
#include "quantiser/QuantisedFrame.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

class LinearReader : public FrameReader
{
    public:
        LinearReader(FILE* input_file);

        ~LinearReader();
        
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
        /* Stored frames to predict on */
        std::deque<QuantisedFrame> frames;
        
        /* Output file */
        FILE* in_file;
        
        /* Number of frames */
        int frames_left;
        
        /* Number of atoms in each frame */
        int atoms;
        
        /* Data from the DCD file header */
        int istart;
        int nsavc;
        double delta;
        
        ArithmeticDecoder decoder;
        AdaptiveModelDecoder model;
};