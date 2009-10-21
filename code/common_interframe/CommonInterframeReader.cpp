#include <cassert>
#include <string>
#include <algorithm>
#include "CommonInterframeReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

using namespace std;

CommonInterframeReader::CommonInterframeReader(FILE* input_file)
 : in_file(input_file), K(2), index_decoder(AdaptiveModelDecoder(&decoder)), error_decoder(AdaptiveModelDecoder(&decoder))
{
    
}

CommonInterframeReader::~CommonInterframeReader()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void CommonInterframeReader::start()
{
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&frames_left, sizeof(unsigned int), 1, in_file);

    // Read data to be able to reconstitute the dcd file on decompression
    fread(&istart, sizeof(int), 1, in_file);
    fread(&nsavc, sizeof(int), 1, in_file);
    fread(&delta, sizeof(double), 1, in_file); 
    
    fread(&K, sizeof(int), 1, in_file); 
    
    decoder.start_decode(in_file);
}

/* Writes a frame */
bool CommonInterframeReader::next_frame(QuantisedFrame& qframe)
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
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            vector< pair<long long, unsigned int> > errors;
            
            for(int j = 0; j < frames.size(); ++j)
            {
                pair<long long, unsigned int> p = make_pair((long long)qframe.quantised_frame[i] - (long long)frames[j].quantised_frame[i], j);
                errors.push_back(p);
            }
            sort(errors.begin(), errors.end());
            
            int best = 0;
            long long error = 0;
            
            sscanf(index_decoder.decode().c_str(), "%d", &best);
            sscanf(error_decoder.decode().c_str(), "%lld", &error);

            qframe.quantised_frame[i] = frames[errors[best].second].quantised_frame[i] + error;
            
            error_frequencies[error]++;
            index_frequencies[best]++;
        }
        
        if(frames.size() == K)
            frames.pop_front();
    }
    else
    {
        AdaptiveModelDecoder model(&decoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
            sscanf(model.decode().c_str(), "%u", &qframe.quantised_frame[i]);
        
    }
    
    frames.push_back(qframe);
    
    return true;
}

/* Ends the reading process. */
void CommonInterframeReader::end()
{
}
