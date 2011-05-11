#pragma once
#include <vector>
#include <deque>
#include <map>
#include <cstdio>
#include "FrameWriter.h"
#include "quantiser/QuantisedFrame.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

class LinearWriter : public FrameWriter
{
    public:
        LinearWriter(FILE* output_file, bool output = false);

        ~LinearWriter();

        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start(int atoms, int frames, int ISTART = 0, int NSAVC = 1, double DELTA = 0.0);

        /* Writes a frame */
        void next_frame(const QuantisedFrame& qframe);

        /* Ends the reading process. */
        void end();
        
    private:
        /* Stored frames to predict on */
        std::deque<QuantisedFrame> frames;
        
        /* Output file */
        FILE* out_file;
        
        ArithmeticEncoder encoder;
        AdaptiveModelEncoder model;
        
        bool do_output;
        std::map<int, long long> errors;
};