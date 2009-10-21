#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

#include "CommonInterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

CommonInterframeWriter::CommonInterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on), index_model(AdaptiveModelEncoder(&encoder)), error_model(AdaptiveModelEncoder(&encoder))
{
}

CommonInterframeWriter::~CommonInterframeWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void CommonInterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
{
    fwrite(&atoms, sizeof(int), 1, out_file);
    fwrite(&frames, sizeof(int), 1, out_file);

    // Write data to be able to reconstitute the dcd file on decompression
    fwrite(&ISTART, sizeof(int), 1, out_file);
    fwrite(&NSAVC, sizeof(int), 1, out_file);
    fwrite(&DELTA, sizeof(double), 1, out_file);
    
    // Write window size
    fwrite(&K, sizeof(int), 1, out_file);
    
    encoder.start_encode(out_file);
}

/* Writes a frame */
void CommonInterframeWriter::next_frame(const QuantisedFrame& qframe)
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
            vector< pair<long long, unsigned int> > errors;
            
            for(int j = 0; j < frames.size(); ++j)
            {
                pair<long long, unsigned int> p = make_pair((long long)qframe.quantised_frame[i] - (long long)frames[j].quantised_frame[i], j);
                errors.push_back(p);
            }
            
            sort(errors.begin(), errors.end());
            int best = 0;
            long long best_freq = (1+error_frequencies[errors[0].first])*(1+index_frequencies[0]);
            
            for(int j = 1; j < errors.size(); ++j)
                if((1+error_frequencies[errors[j].first])*(1+index_frequencies[j] > best_freq))
                {
                    best = j;
                    best_freq = (1+error_frequencies[errors[j].first])*(1+index_frequencies[j]);
                }
                
            int output1 = best;
            long long output2 = errors[best].first;
            
            index_model.encode_bytes(&output1, sizeof(output1));
            error_model.encode_bytes(&output2, sizeof(output2));
            
            error_frequencies[errors[best].first]++;
            index_frequencies[best]++;
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
void CommonInterframeWriter::end()
{
    encoder.end_encode();
}
