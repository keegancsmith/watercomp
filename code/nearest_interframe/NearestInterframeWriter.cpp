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
    
    fwrite(&K, sizeof(int), 1, out_file);
    
    encoder.start_encode(out_file);
}

/* Writes a frame */
void NearestInterframeWriter::next_frame(const QuantisedFrame& qframe)
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
            vector<int> items;
            
            for(int j = 0; j < K; ++j)
                items.push_back(frames[j].quantised_frame[i]);
            
            sort(items.begin(), items.end());
            
            int nearest = 0;
            int nearest_distance = fabs(items[0] - (int)qframe.quantised_frame[i]);
            
            for(int j = 1; j < K; ++j)
                if(fabs(items[j] - (int)qframe.quantised_frame[i]) < nearest_distance)
                {
                    nearest = j;
                    nearest_distance = fabs(items[j] - (int)qframe.quantised_frame[i]);
                }
                
            int output1 = nearest;
            int output2 = items[nearest] - (int)qframe.quantised_frame[i];
            
            index_model.encode_bytes(&output1, sizeof(output1));
            error_model.encode_bytes(&output2, sizeof(output2));
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
void NearestInterframeWriter::end()
{
    encoder.end_encode();
}
