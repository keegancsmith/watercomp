#include <cassert>
#include <string>
#include <algorithm>
#include "SplineInterframeReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

using namespace std;

SplineInterframeReader::SplineInterframeReader(FILE* input_file, int predict_on)
 : in_file(input_file), K(predict_on), error_model(AdaptiveModelDecoder(&decoder))
{
    factorials.push_back(1.0);
    
    for(int i = 1; i < K; ++i)
        factorials.push_back(factorials[factorials.size()-1]*i);
}

SplineInterframeReader::~SplineInterframeReader()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void SplineInterframeReader::start()
{
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&frames_left, sizeof(unsigned int), 1, in_file);

    // Write data to be able to reconstitute the dcd file on decompression
    fread(&istart, sizeof(int), 1, in_file);
    fread(&nsavc, sizeof(int), 1, in_file);
    fread(&delta, sizeof(double), 1, in_file); 
    
    decoder.start_decode(in_file);
}

/* Writes a frame */
bool SplineInterframeReader::next_frame(QuantisedFrame& qframe)
{
    if(frames_left == 0)
        return false;
    
    --frames_left;
    
    /// Read frame header: Quantisation and bounding box
    AdaptiveModelDecoder initial(&decoder);
    
    sscanf(initial.decode().c_str(), "%u", &qframe.m_xquant);
    sscanf(initial.decode().c_str(), "%u", &qframe.m_yquant);
    sscanf(initial.decode().c_str(), "%u", &qframe.m_zquant);
    
    for(int i = 0; i < 3; ++i)
        sscanf(initial.decode().c_str(), "%f", qframe.min_coord+i);
    
    for(int i = 0; i < 3; ++i)
        sscanf(initial.decode().c_str(), "%f", qframe.max_coord+i);

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
            
            int error = 0;
            sscanf(error_model.decode().c_str(), "%d", &error);
            qframe.quantised_frame[i] = int(guess + 0.5) + error;
        }
        
        if(frames.size() == K)
            frames.pop_front();
    }
    else
    {
        AdaptiveModelDecoder model(&decoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
            sscanf(model.decode().c_str(), "%d", &qframe.quantised_frame[i]);
    }
    
    frames.push_back(qframe);
    
    return true;
}

/* Ends the reading process. */
void SplineInterframeReader::end()
{
}