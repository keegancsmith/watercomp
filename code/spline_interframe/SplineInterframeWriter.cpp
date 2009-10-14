#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

#include "SplineInterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

SplineInterframeWriter::SplineInterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on), index_model(AdaptiveModelEncoder(&encoder)), error_model(AdaptiveModelEncoder(&encoder))
{
    factorials.push_back(1.0);
    
    for(int i = 1; i < K; ++i)
        factorials.push_back(factorials[factorials.size()-1]*i);
}

SplineInterframeWriter::~SplineInterframeWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void SplineInterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
{
    fwrite(&atoms, sizeof(int), 1, out_file);
    fwrite(&frames, sizeof(int), 1, out_file);

    // Write data to be able to reconstitute the dcd file on decompression
    fwrite(&ISTART, sizeof(int), 1, out_file);
    fwrite(&NSAVC, sizeof(int), 1, out_file);
    fwrite(&DELTA, sizeof(double), 1, out_file);
    
    encoder.start_encode(out_file);
}

/* Writes a frame */
void SplineInterframeWriter::next_frame(const QuantisedFrame& qframe)
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
        double t = double(K + 1)/double(K);
        double mt = 1.0 - t;
        
        double ct = 1.0;
        double ft = 1.0;
        
        int k = K - 1;
        double cur = 1.0 - t;
        
        while(k)
        {
            if(k % 2 == 1)
                ft *= cur;
                
            k /= 2;
            
            cur *= cur;
        }
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            double guess = 0.0;
            double increasing_t = ct;
            double decreasing_t = ft;
            
            for(int j = 0; j < K; ++j)
            {
                guess += factorials[K-1]/(factorials[j]*factorials[K-1-j])*increasing_t*decreasing_t*frames[j].quantised_frame[i];
                
                increasing_t *= t;
                decreasing_t /= mt;
            }
            
            
            sprintf(buffer, "%d", int(qframe.quantised_frame[i]) - int(guess + 0.5));
            error_model.encode(buffer);
        }
        
        if(frames.size() == K)
            frames.pop_front();
    }
    else
    {
        AdaptiveModelEncoder model(&encoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            sprintf(buffer, "%d", qframe.quantised_frame[i]);
            model.encode(buffer);
        }
        
    }
    
    frames.push_back(qframe);
}

/* Ends the reading process. */
void SplineInterframeWriter::end()
{
    encoder.end_encode();
}
