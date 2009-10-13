#include <cassert>
#include <string>
#include <algorithm>
#include <cmath>
#include "NearestInterframeReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

using namespace std;

InterframeReader::InterframeReader(FILE* input_file, int predict_on)
 : in_file(input_file), K(predict_on), index_model(&decoder), error_model(&decoder)
{
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
            vector<int> items;
            
            for(int j = 0; j < K; ++j)
                items.push_back(frames[j].quantised_frame[i]);
            
            sort(items.begin(), items.end());
            
            int index = 0;
            double error = 0;
            
            sscanf(index_model.decode().c_str(), "%d", &index);
            sscanf(error_model.decode().c_str(), "%lf", &error);
            
            qframe.quantised_frame[i] = frames[index].quantised_frame[i] - error;
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
void InterframeReader::end()
{
}