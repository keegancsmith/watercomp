#include "OmelReader.h"
#include <vector>
#include <string>
#include "SortedQuantisedFrame.h"
#include <utility>

using namespace std;

/* Starts the reading process. Also save start frame, steps between each save and delta from dcd header. */
void OmelReader::start()
{
   // Reads the number of atoms and the number of frames
    // Expects there to be same number of atoms in each frame.
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&frames_left, sizeof(unsigned int), 1, in_file);
    fread(&istart, sizeof(int), 1, in_file);
    fread(&nsavc, sizeof(int), 1, in_file);
    fread(&delta, sizeof(double), 1, in_file); 
}
        
/* Reads a frame */
bool OmelReader::next_frame(QuantisedFrame& qframe)
{
    if(frames_left == 0)
        return false;
    
    --frames_left;
    
    SortedQuantisedFrame sqframe;
    
    fread(&sqframe.m_xquant, sizeof(unsigned int), 1, in_file);
    fread(&sqframe.m_yquant, sizeof(unsigned int), 1, in_file);
    fread(&sqframe.m_zquant, sizeof(unsigned int), 1, in_file);
    
    fread(sqframe.min_coord, sizeof(float), 3, in_file);
    fread(sqframe.max_coord, sizeof(float), 3, in_file);
    
    queue<bool> bit_buffer;
        
    OmelDecoder order_decoder(in_file, &bit_buffer);
    OmelDecoder dist_decoder(in_file, &bit_buffer);
    
    unsigned long long last = 0;
    
    for(int i = 0; i < atoms; ++i)
    {
        unsigned long long index = last + dist_decoder.read_uint64();
        unsigned long long order = order_decoder.read_uint64();
        
        sqframe.sorted_frame.push_back(make_pair(index, order));
        
        last = index;
    }
    
    qframe = sqframe.toQFrame();
    
    return true;
}

/* Ends the reading process. */
void OmelReader::end()
{
}
