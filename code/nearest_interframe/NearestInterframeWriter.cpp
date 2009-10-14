#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

#include "NearestInterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

NearestInterframeWriter::NearestInterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on), index_model(AdaptiveModelEncoder(&encoder)), error_model(AdaptiveModelEncoder(&encoder))
{
}

NearestInterframeWriter::~NearestInterframeWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void NearestInterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
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
void NearestInterframeWriter::next_frame(const QuantisedFrame& qframe)
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
            vector<int> items;
            
            for(int j = 0; j < K; ++j)
                items.push_back(frames[j].quantised_frame[i]);
            
            sort(items.begin(), items.end());
            
            int nearest = 0;
            int nearest_distance = fabs(items[0] - qframe.quantised_frame[i]);
            
            for(int j = 1; j < K; ++j)
                if(fabs(items[j] - qframe.quantised_frame[i]) < nearest_distance)
                {
                    nearest = j;
                    nearest_distance = fabs(items[j] - qframe.quantised_frame[i]);
                }
            
            sprintf(buffer, "%d", nearest);
            index_model.encode(buffer);
            
            sprintf(buffer, "%lf", double((long long)items[nearest] - (long long)qframe.quantised_frame[i]));
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
void NearestInterframeWriter::end()
{
    encoder.end_encode();
}
