#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

#include "NNInterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

NNInterframeWriter::NNInterframeWriter(FILE* output_file, int predict_on, int vector_size)
 : out_file(output_file), K(predict_on), V(vector_size), error_model(AdaptiveModelEncoder(&encoder))
{
}

NNInterframeWriter::~NNInterframeWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void NNInterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
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
void NNInterframeWriter::next_frame(const QuantisedFrame& qframe)
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
            int nearest_index = 0;
            long long distance = 1LL<<60;
            
            for(int j = 0; j < frames.size() - V - 1; ++j)
            {
                long long this_distance = 0;
                
                for(int k = 0; k < V; ++k)
                {
                    long long delta = (long long)frames[j+k].quantised_frame[i] - (long long)frames[frames.size() - V + k].quantised_frame[i];
                    
                    this_distance += delta*delta;
                }
                
                if(this_distance < distance)
                {
                    nearest_index = j;
                    distance = this_distance;
                }
            }
            
            sprintf(buffer, "%lf", double(frames[nearest_index + V].quantised_frame[i] - qframe.quantised_frame[i]));
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
void NNInterframeWriter::end()
{
    encoder.end_encode();
}
