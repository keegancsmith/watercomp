#include <cassert>
#include <string>
#include "InterframeReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

using namespace std;

InterframeReader::InterframeReader(FILE* input_file)
 : in_file(input_file), K(2), model(&decoder)
{
    factorial = 1;
    
    for(int i = 1; i <= K; ++i)
        factorial *= i;
    
    for(int j = 0; j <= 170; ++j)
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

InterframeReader::~InterframeReader()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void InterframeReader::start()
{
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&frames_left, sizeof(unsigned int), 1, in_file);

    // Write data to be able to reconstitute the dcd file on decompression
    fread(&istart, sizeof(int), 1, in_file);
    fread(&nsavc, sizeof(int), 1, in_file);
    fread(&delta, sizeof(double), 1, in_file);
    
    fread(&K, sizeof(int), 1, in_file);
    
    factorial = 1.0;
    for(int i = 1; i <= K; ++i)
        factorial *= i;
    
    weights.clear();
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
    
    decoder.start_decode(in_file);
}

/* Writes a frame */
bool InterframeReader::next_frame(QuantisedFrame& qframe)
{
    if(frames_left == 0)
        return false;
    
    --frames_left;
    
    /// Read frame header: Quantisation and bounding box
    AdaptiveModelDecoder initial(&decoder);
    initial.decode_bytes(&qframe.m_xquant);
    initial.decode_bytes(&qframe.m_yquant);
    initial.decode_bytes(&qframe.m_zquant);
    
    int box[3] = {1<<qframe.m_xquant, 1<<qframe.m_yquant, 1<<qframe.m_zquant};
    
    for(int i = 0; i < 3; ++i)
        initial.decode_bytes(qframe.min_coord+i);

    for(int i = 0; i < 3; ++i)
        initial.decode_bytes(qframe.max_coord+i);

    if(frames.size() == K)
    {
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            int input;
            double estimated = 0;
            
            model.decode_bytes(&input);
            
            for(int j = 0; j < K; ++j)
            {
                double l_j = (factorial/(K - j))/weights[j];
                estimated += l_j*frames[j].quantised_frame[i];
            }
            
            int guess = max(0, min(box[i%3], int(estimated + 0.5)));
            
            qframe.quantised_frame[i] = guess - input;
        }
        
        if(!frames.empty())
            frames.pop_front();
    }
    else
    {
        AdaptiveModelDecoder window_model(&decoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
            window_model.decode_bytes(&qframe.quantised_frame[i]);
    }
    
    frames.push_back(qframe);
    
    return true;
}

/* Ends the reading process. */
void InterframeReader::end()
{
}
