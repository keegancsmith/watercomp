#include <cassert>
#include "InterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

InterframeWriter::InterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on), model(&encoder)
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
    
    fwrite(&K, sizeof(int), 1, out_file);
    
    encoder.start_encode(out_file);
}

/* Writes a frame */
void InterframeWriter::next_frame(const QuantisedFrame& qframe)
{
    char buffer[100];
    
    /// Write frame header: Quantisation and bounding box
    AdaptiveModelEncoder initial(&encoder);
    sprintf(buffer, "%u", qframe.m_xquant);
    initial.encode(buffer);
    
    sprintf(buffer, "%u", qframe.m_yquant);
    initial.encode(buffer);
    
    sprintf(buffer, "%u", qframe.m_zquant);
    initial.encode(buffer);
    
    for(int i = 0; i < 3; ++i)
    {
        sprintf(buffer, "%f", qframe.min_coord[i]);
        initial.encode(buffer);
    }
    
    for(int i = 0; i < 3; ++i)
    {
        sprintf(buffer, "%f", qframe.max_coord[i]);
        initial.encode(buffer);
    }
    
    if(frames.size() == K)
    {
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            double estimated = 0;
            
            for(int j = 0; j < K; ++j)
            {
                double l_j = (factorial/(K - j))/weights[j];
                estimated += l_j*frames[j].quantised_frame[i];
            }

            int guess = int(estimated + 0.5);
            
            sprintf(buffer, "%d", guess - int(qframe.quantised_frame[i]));
            model.encode(buffer);
        }
        
        if(!frames.empty())
            frames.pop_front();
    }
    else
    {
        AdaptiveModelEncoder model(&encoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
//             sprintf(buffer, "%d", qframe.quantised_frame[i]);
//             model.encode(buffer);
            model.encode_bytes((char*)(&qframe.quantised_frame[i]), 4);
        }
        
    }
    
    frames.push_back(qframe);
}

/* Ends the reading process. */
void InterframeWriter::end()
{
    encoder.end_encode();
}
