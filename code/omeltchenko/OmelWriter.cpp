#include <cstdio>
#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
//#include "Atom.h"
#include "OmelWriter.h"
#include "OmelEncoder.h"
#include "quantiser/QuantisedFrame.h"
#include "SortedQuantisedFrame.h"

#include <string>
using namespace std;

OmelWriter::~OmelWriter()
{
}

void OmelWriter::start(int atoms, int frames, int ISTART, int NSAVC, double DELTA)
{
    fwrite(&atoms, sizeof(int), 1, out_file);
    fwrite(&frames, sizeof(int), 1, out_file);
    
    // Write data to be able to reconstitute the dcd file on decompression
    fwrite(&ISTART, sizeof(int), 1, out_file);
    fwrite(&NSAVC, sizeof(int), 1, out_file);
    fwrite(&DELTA, sizeof(double), 1, out_file);
}
        
void OmelWriter::next_frame(const QuantisedFrame & qframe)
{
    SortedQuantisedFrame sqframe(qframe);
    
    /// Write frame header: Quantisation and bounding box
    fwrite(&qframe.m_xquant, sizeof(unsigned int), 1, out_file);
    fwrite(&qframe.m_yquant, sizeof(unsigned int), 1, out_file);
    fwrite(&qframe.m_zquant, sizeof(unsigned int), 1, out_file);
    
    fwrite(qframe.min_coord, sizeof(float), 3, out_file);
    fwrite(qframe.max_coord, sizeof(float), 3, out_file);
    
    queue<bool> buffer;
    
    /// Need to compress the index and ordering information
    OmelEncoder index_encoder(out_file, &buffer);
    OmelEncoder order_encoder(out_file, &buffer);
    
    
    unsigned long long last_octree_index = 0;
    
    for(int i = 0; i < sqframe.sorted_frame.size(); ++i)
    {
        unsigned long long this_octree_index = sqframe.sorted_frame[i].first;
        
        index_encoder.write_uint64(this_octree_index - last_octree_index);
        order_encoder.write_uint64(sqframe.sorted_frame[i].second);
        
        last_octree_index = this_octree_index;
    }
        
    /// Flush bit pool
    while(buffer.size() % 8 != 0)
        buffer.push(0);
    
    while(buffer.size() >= 8)
    {
        unsigned char acc = 0;
        
        for(int i = 0; i < 8; ++i)
        {
            acc = (acc << 1) | buffer.front();
            buffer.pop();
        }
        
        fprintf(out_file, "%c", acc);
    }
}

void OmelWriter::end()
{
}
