#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

#include "SplineInterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

SplineInterframeWriter::SplineInterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on), error_model(AdaptiveModelEncoder(&encoder))
{
    factorials.push_back(1.0);
    
    for(int i = 1; i < K; ++i)
        factorials.push_back(factorials[factorials.size()-1]*i);
}

SplineInterframeWriter::~SplineInterframeWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void SplineInterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
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
void SplineInterframeWriter::next_frame(const QuantisedFrame& qframe)
{
    char buffer[100];
    
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
        double t = double(K + 1)/double(K);
        double mt = 1.0 - t;
        
        double ct = 1.0;
        double ft = 1.0;
        
        int k = K - 1;
        double cur = 1.0 - t;
        
        while(k)
        {
            if(k % 2 == 1)
                ft *= cur;
                
            k /= 2;
            
            cur *= cur;
        }
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            double guess = 0.0;
            double increasing_t = ct;
            double decreasing_t = ft;
            
            for(int j = 0; j < K; ++j)
            {
                guess += factorials[K-1]/(factorials[j]*factorials[K-1-j])*increasing_t*decreasing_t*frames[j].quantised_frame[i];
                
                increasing_t *= t;
                decreasing_t /= mt;
            }
            
            int out_guess = max(0, min(box[i%3], int(guess + 0.5)));
            int output = int(qframe.quantised_frame[i]) - out_guess;
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
void SplineInterframeWriter::end()
{
    encoder.end_encode();
}
