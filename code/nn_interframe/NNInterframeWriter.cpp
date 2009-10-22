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
    
    fwrite(&K, sizeof(int), 1, out_file);
    fwrite(&V, sizeof(int), 1, out_file);
    
    encoder.start_encode(out_file);
}

/* Writes a frame */
void NNInterframeWriter::next_frame(const QuantisedFrame& qframe)
{
    /// Write frame header: Quantisation and bounding box
    AdaptiveModelEncoder initial(&encoder);
    initial.encode_bytes(&qframe.m_xquant, sizeof(qframe.m_xquant));
    initial.encode_bytes(&qframe.m_yquant, sizeof(qframe.m_yquant));
    initial.encode_bytes(&qframe.m_zquant, sizeof(qframe.m_zquant));
    
    int box[3] = {1<<qframe.m_xquant, 1<<qframe.m_yquant, 1<<qframe.m_zquant};
    
    for(int i = 0; i < 3; ++i)
        initial.encode_bytes(&qframe.min_coord[i], sizeof(qframe.min_coord[i]));
    
    for(int i = 0; i < 3; ++i)
        initial.encode_bytes(&qframe.max_coord[i], sizeof(qframe.max_coord[i]));
    
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
            
            int output = int(frames[nearest_index + V].quantised_frame[i]) - int(qframe.quantised_frame[i]);
            error_model.encode_bytes(&output, sizeof(output));
        }
        
        if(frames.size() == K)
            frames.pop_front();
    }
    else
    {
        AdaptiveModelEncoder window_model(&encoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
            window_model.encode_bytes(&qframe.quantised_frame[i], sizeof(qframe.quantised_frame[i]));
        
    }
    
    frames.push_back(qframe);
}

/* Ends the reading process. */
void NNInterframeWriter::end()
{
    encoder.end_encode();
}
