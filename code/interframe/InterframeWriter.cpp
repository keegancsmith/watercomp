#include <cassert>
#include "InterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

InterframeWriter::InterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on)
{
    factorial = 1;
    
    // Only allow upto 170 frames to be predicted on to avoid 
    assert(0 <= K && K <= 170);
    
    for(int i = 1; i <= K; ++i)
        factorial *= i;
    
    for(int j = 0; j < K; ++j)
    {
        double weight = 1;
        
        for(int i = 0; i < K; ++i)
        {
            if(i == j)
                continue;
            
            weight *= (j - i);
        }
        
        weights.push_back(weight);
    }
}

InterframeWriter::~InterframeWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void InterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
{
    fwrite(&atoms, sizeof(int), 1, out_file);
    fwrite(&frames, sizeof(int), 1, out_file);

    // Write data to be able to reconstitute the dcd file on decompression
    fwrite(&ISTART, sizeof(int), 1, out_file);
    fwrite(&NSAVC, sizeof(int), 1, out_file);
    fwrite(&DELTA, sizeof(double), 1, out_file);
}

/* Writes a frame */
void InterframeWriter::next_frame(const QuantisedFrame& qframe)
{
    if(frames.size() == K)
    {
        char buffer[100];
        
        /// Write frame header: Quantisation and bounding box
        fwrite(&qframe.m_xquant, sizeof(unsigned int), 1, out_file);
        fwrite(&qframe.m_yquant, sizeof(unsigned int), 1, out_file);
        fwrite(&qframe.m_zquant, sizeof(unsigned int), 1, out_file);

        fwrite(qframe.min_coord, sizeof(float), 3, out_file);
        fwrite(qframe.max_coord, sizeof(float), 3, out_file);
        
        ArithmeticEncoder encoder;
        AdaptiveModelEncoder model(&encoder);
        
        encoder.start_encode(out_file);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            double estimated = -double(qframe.quantised_frame[i]);
            
            for(int j = 0; j < K; ++j)
            {
                double summand = double(frames[j].quantised_frame[i])/weights[j];
                estimated += (factorial/(K-j))*summand;
            }
            
            float output = estimated;
            sprintf(buffer, "%f", output);
            
            model.encode(buffer);
        }
        
        encoder.end_encode();
        
        if(!frames.empty())
            frames.pop_front();
    }
    else
    {
        char buffer[1000];
        ArithmeticEncoder encoder;
        AdaptiveModelEncoder model(&encoder);
        
        encoder.start_encode(out_file);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            sprintf(buffer, "%d", qframe.quantised_frame[i]);
            model.encode(buffer);
        }
        
        encoder.end_encode();
    }
    
    frames.push_back(qframe);
}

/* Ends the reading process. */
void InterframeWriter::end()
{
}