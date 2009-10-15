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
            vector< pair<double, unsigned int> > errors;
            
            for(int j = 0; j < frames.size(); ++j)
            {
                pair<double, unsigned int> p = make_pair((long long)qframe.quantised_frame[i] - (long long)frames[j].quantised_frame[i], j);
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
            
            sprintf(buffer, "%d", best);
            index_model.encode(buffer);
            sprintf(buffer, "%lf", errors[best].first);
            error_model.encode(buffer);
            
            error_frequencies[errors[best].first]++;
            index_frequencies[best]++;
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
void CommonInterframeWriter::end()
{
    encoder.end_encode();
}
