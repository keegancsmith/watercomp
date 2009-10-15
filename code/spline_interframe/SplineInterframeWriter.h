#pragma once
#include <vector>
#include <deque>
#include <cstdio>
#include <map>
#include "FrameWriter.h"
#include "quantiser/QuantisedFrame.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

class SplineInterframeWriter : public FrameWriter
{
    public:
        SplineInterframeWriter(FILE* output_file, int predict_on);

        ~SplineInterframeWriter();

        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start(int atoms, int frames, int ISTART = 0, int NSAVC = 1, double DELTA = 0.0);

        /* Writes a frame */
        void next_frame(const QuantisedFrame& qframe);

        /* Ends the reading process. */
        void end();
        
    private:
        
        /* The amount of frames to predict with */
        int K;
        
        std::vector<double> factorials;
        
        /* Stored frames to predict on */
        std::deque<QuantisedFrame> frames;
        
        /* Output file */
        FILE* out_file;
        
        ArithmeticEncoder encoder;
        AdaptiveModelEncoder error_model;
};