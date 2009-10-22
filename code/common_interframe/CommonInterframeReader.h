#pragma once
#include <vector>
#include <deque>
#include <cstdio>
#include "FrameReader.h"
#include "quantiser/QuantisedFrame.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

class CommonInterframeReader : public FrameReader
{
    public:
        CommonInterframeReader(FILE* input_file);

        ~CommonInterframeReader();
        
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
        
        /* The amount of frames to predict with */
        int K;
        
        /* Precomputed value for Lagrange interpolating polynomial */
        double factorial;
        std::vector<double> weights;
        
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
        
        AdaptiveModelDecoder index_decoder;
        AdaptiveModelDecoder error_decoder;
        
        std::map<long long, unsigned int> error_frequencies;
        std::map<unsigned int, unsigned int> index_frequencies;
};
