#include <cassert>
#include "LinearWriter.h"
#include "arithmetic/ArithmeticEncoder.h"
#include "arithmetic/AdaptiveModelEncoder.h"

using namespace std;

LinearWriter::LinearWriter(FILE* output_file, bool output)
 : do_output(output), out_file(output_file), model(&encoder)
{
}

LinearWriter::~LinearWriter()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void LinearWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
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
void LinearWriter::next_frame(const QuantisedFrame& qframe)
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
    
    if(frames.size() == 2)
    {
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            int predicted = 2*frames[1].quantised_frame[i] - frames[0].quantised_frame[i];
            int output = qframe.quantised_frame[i] - predicted;
            
            if(output >= box[i%3]/2)
                output = output - box[i%3];
            else if(output <= -box[i%3]/2)
                output = output + box[i%3];
            
            model.encode_bytes(&output, sizeof(output));
            
            if(do_output)
                ++errors[output];
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
}

/* Ends the reading process. */
void LinearWriter::end()
{
    encoder.end_encode();
    
    if(do_output)
    {
        FILE* stat_file = fopen("errors.csv", "w");
        
        fprintf(stat_file, "value, count\n");
        
        for(map<int, long long>::iterator it = errors.begin(); it != errors.end(); ++it)
            fprintf(stat_file, "%d, %lld\n", it->first, it->second);
        
        fclose(stat_file);
    }
}
