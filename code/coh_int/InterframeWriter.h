#pragma once
#include <vector>
#include <deque>
#include <map>
#include <cstdio>
#include "FrameWriter.h"
#include "quantiser/QuantisedFrame.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

class InterframeWriter : public FrameWriter
{
    public:
        InterframeWriter(FILE* output_file, int predict_on);

        ~InterframeWriter();

        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start(int atoms, int frames, int ISTART = 0, int NSAVC = 1, double DELTA = 0.0);

        /* Writes a frame */
        void next_frame(const QuantisedFrame& qframe);

        /* Ends the reading process. */
        void end();
        
    private:
        
        /* The amount of frames to predict with */
        int K;
        
        /* Precomputed value for Lagrange interpolating polynomial */
        double factorial;
        std::vector<double> weights;
        
        /* Stored frames to predict on */
        std::deque<QuantisedFrame> frames;
        
        /* Output file */
        FILE* out_file;
        
        FILE* log_file;
        std::map<int, int> error_frequencies;
        
        int f_written;
        
        ArithmeticEncoder encoder;
        AdaptiveModelEncoder model;
};