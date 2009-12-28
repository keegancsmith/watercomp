#include <cassert>
#include "InterframeWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

InterframeWriter::InterframeWriter(FILE* output_file, int predict_on)
 : out_file(output_file), K(predict_on), model(&encoder)
{
    factorial = 1;
    
    // Only allow upto 170 frames to be predicted on to avoid 
    assert(0 <= K && K <= 170);
    
    for(int i = 1; i <= K; ++i)
        factorial *= i;
    
    for(int j = 0; j < K; ++j)
    {
        double weight = 1;
        
        for(int i = 0; i < K; ++i)
        {
            if(i == j)
                continue;
            
            weight *= (j - i);
        }
        
        weights.push_back(weight);
    }
    
    log_file = fopen("comp.log", "w");
    
    f_written = 0;
    
}

InterframeWriter::~InterframeWriter()
{
    fprintf(log_file, "=== ERROR FREQUENCY ===\n");
    fprintf(log_file, "Distinct Errors: %d\n", error_frequencies.size());
    
    for(map<int, int>::iterator it = error_frequencies.begin(); it != error_frequencies.end(); ++it)
        fprintf(log_file, "Error %d; Count %d\n", it->first, it->second);
    
    fprintf(log_file, "\n");
    
    fclose(log_file);
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void InterframeWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
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
void InterframeWriter::next_frame(const QuantisedFrame& qframe)
{
    fprintf(log_file, "=== Frame %d ===\n", f_written);
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
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            double estimated = 0;
            
            for(int j = 0; j < K; ++j)
            {
                double l_j = (factorial/(K - j))/weights[j];
                estimated += l_j*frames[j].quantised_frame[i];
            }
            
//             int guess = max(0, min(box[i%3], int(estimated + 0.5)));
            int guess = int(estimated + 0.5);
            int output = guess - int(qframe.quantised_frame[i]);
            
            if(output >= box[i%3]/2)
            {
                output = output - box[i%3];
//                 output = 0;
            }
            else if(output <= -box[i%3]/2)
            {
                output = -box[i%3] - output;
//                 output = 0;
            }
//             if(output*output > 200*200)
//             {
//                 fprintf(log_file, "Anomaly Detected! Output %d, Atom %d, Coordinate %d\n", i, i/3, i%3);
//                 fprintf(log_file, "Anomaly: Estimated %lf, Error %d, Actual %d, Box %d\n", estimated, output, qframe.quantised_frame[i], box[i%3]);
//                 
//                 fprintf(log_file, "Anomaly Window:");
//                 
//                 for(int j = 0; j < K; ++j)
//                     fprintf(log_file, " %d", frames[j].quantised_frame[i]);
//                 fprintf(log_file, "\n");
//             }
            ++error_frequencies[output];
            model.encode_bytes(&output, sizeof(output));
        }
        
        if(!frames.empty())
            frames.pop_front();
    }
    else
    {
        AdaptiveModelEncoder window_model(&encoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
            window_model.encode_bytes(&qframe.quantised_frame[i], sizeof(qframe.quantised_frame[i]));
    }
    
    frames.push_back(qframe);
    
    ++f_written;
}

/* Ends the reading process. */
void InterframeWriter::end()
{
    encoder.end_encode();
}
