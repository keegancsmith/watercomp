#include <cassert>
#include <string>
#include <algorithm>
#include "NNInterframeReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

using namespace std;

NNInterframeReader::NNInterframeReader(FILE* input_file)
 : in_file(input_file), K(2), V(1), error_model(AdaptiveModelDecoder(&decoder))
{
    
}

NNInterframeReader::~NNInterframeReader()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void NNInterframeReader::start()
{
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&frames_left, sizeof(unsigned int), 1, in_file);

    // Write data to be able to reconstitute the dcd file on decompression
    fread(&istart, sizeof(int), 1, in_file);
    fread(&nsavc, sizeof(int), 1, in_file);
    fread(&delta, sizeof(double), 1, in_file); 
    
    fread(&K, sizeof(int), 1, in_file);
    fread(&V, sizeof(int), 1, in_file);
    
    
    decoder.start_decode(in_file);
}

/* Writes a frame */
bool NNInterframeReader::next_frame(QuantisedFrame& qframe)
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
            int nearest_index = 0;
            long long distance = 1LL<<60;
            
            for(int j = 0; j < frames.size() - V - 1; ++j)
            {
                long long this_distance = 0;
                
                for(int k = 0; k < V; ++k)
                {
                    long long delta = (long long)frames[j + k].quantised_frame[i] - (long long)frames[frames.size() - V + k].quantised_frame[i];
                    
                    this_distance += delta*delta;
                }
                
                if(this_distance < distance)
                {
                    nearest_index = j;
                    distance = this_distance;
                }
            }
            int error = 0;
            error_model.decode_bytes(&error);
            qframe.quantised_frame[i] = frames[nearest_index + V].quantised_frame[i] - error;
        }
        
        if(frames.size() == K)
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
void NNInterframeReader::end()
{
}
