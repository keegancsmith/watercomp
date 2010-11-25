#include <cassert>
#include <string>
#include "LinearReader.h"
#include "arithmetic/ArithmeticDecoder.h"
#include "arithmetic/AdaptiveModelDecoder.h"

using namespace std;

LinearReader::LinearReader(FILE* input_file)
 : in_file(input_file), model(&decoder)
{
}

LinearReader::~LinearReader()
{
}

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void LinearReader::start()
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
bool LinearReader::next_frame(QuantisedFrame& qframe)
{
    if(frames_left == 0)
        return false;
    
    --frames_left;
    
    /// Read frame header: Quantisation and bounding box
    AdaptiveModelDecoder initial(&decoder);
    initial.decode_bytes(&qframe.m_xquant);
    initial.decode_bytes(&qframe.m_yquant);
    initial.decode_bytes(&qframe.m_zquant);
    
    int box[3] = {1<<qframe.m_xquant, 1<<qframe.m_yquant, 1<<qframe.m_zquant};
    
    for(int i = 0; i < 3; ++i)
        initial.decode_bytes(qframe.min_coord+i);

    for(int i = 0; i < 3; ++i)
        initial.decode_bytes(qframe.max_coord+i);

    if(frames.size() == 2)
    {
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
        {
            int input;
            model.decode_bytes(&input);
            int guess = 2*frames[1].quantised_frame[i] - frames[0].quantised_frame[i];
            
            qframe.quantised_frame[i] = (guess + input + box[i%3]) % box[i%3];
        }
        
        if(!frames.empty())
            frames.pop_front();
    }
    else
    {
        AdaptiveModelDecoder window_model(&decoder);
        
        for(int i = 0; i < qframe.quantised_frame.size(); ++i)
            window_model.decode_bytes(&qframe.quantised_frame[i]);
    }
    
    frames.push_back(qframe);
    
    return true;
}

/* Ends the reading process. */
void LinearReader::end()
{
}
