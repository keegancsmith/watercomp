#pragma once
#include <vector>
#include <deque>
#include <cstdio>
#include <map>
#include <fann.h>
#include "FrameWriter.h"
#include "quantiser/QuantisedFrame.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

class NNInterframeWriter : public FrameWriter
{
    public:
        NNInterframeWriter(FILE* output_file, int predict_on, int vector_size);

        ~NNInterframeWriter();

        /* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
        void start(int atoms, int frames, int ISTART = 0, int NSAVC = 1, double DELTA = 0.0);

        /* Writes a frame */
        void next_frame(const QuantisedFrame& qframe);

        /* Ends the reading process. */
        void end();
        
    private:
        
        /* The amount of frames to predict with */
        int K;
        
        /* Size of vector < K*/
        int V;
        
        
        /* Stored frames to predict on */
        std::deque<QuantisedFrame> frames;
        
        /* Output file */
        FILE* out_file;
        
        ArithmeticEncoder encoder;
        
        AdaptiveModelEncoder error_model;
};