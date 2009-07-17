#include <cstdio>
#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
#include "Atom.h"
#include "OmelWriter.h"
#include "OmelEncoder.h"

#include <string>
using namespace std;

OmelWriter::~OmelWriter()
{
    // Destroy buckets
    if(sort_buckets != NULL)
        delete [] sort_buckets;
}

void OmelWriter::start_write(int ISTART, int NSAVC, double DELTA)
{
    // Write 0 atoms and 0 frames so far
    unsigned int tmp = 0;
    fwrite(&tmp, sizeof(unsigned int), 1, out_file);
    fwrite(&tmp, sizeof(unsigned int), 1, out_file);
    
    // Write data to be able to reconstitute the dcd file on decompression
    fwrite(&ISTART, sizeof(int), 1, out_file);
    fwrite(&NSAVC, sizeof(int), 1, out_file);
    fwrite(&DELTA, sizeof(double), 1, out_file);
    
}

void OmelWriter::flush()
{
    // Flush allfull bytes out to the file
    while(bit_buffer.size() >= 8)
    {
        unsigned char acc = 0;
        for(int i = 0; i < 8; ++i)
        {
            acc = (acc << 1) | bit_buffer.front();
            bit_buffer.pop();
        }
        fprintf(out_file, "%c", acc);
    }
}

void OmelWriter::end_write()
{
    // Pad buffer, flush and pad the buffer
    while(bit_buffer.size() % 8 != 0)
        bit_buffer.push(0);
    
    flush();
    fclose(out_file);
}

bool OmelWriter::write_frame(vector<Atom>& atom_list, int x_step_bits, int y_step_bits, int z_step_bits)
{
    // One more frame
    ++frames_written;
    
    // Update Frames and Atoms in file
    fseek(out_file, 0, SEEK_SET);
    atoms = atom_list.size();
    fwrite(&atoms, sizeof(unsigned int), 1, out_file);
    fwrite(&frames_written, sizeof(unsigned int), 1, out_file);
    fseek(out_file, 0, SEEK_END);
    
    // Initialise order stuff
    if(order.size() != atoms)
        order.resize(atoms);
//         for(unsigned int i = 0; i < atoms; ++i)
//             order.push_back(i);
    
//     printf("%u\n", order[0]);
        
    // Find the bounding box of the atoms
    float min_x = numeric_limits<float>::max();
    float min_y = numeric_limits<float>::max();
    float min_z = numeric_limits<float>::max();
    
    float max_x = -numeric_limits<float>::max();
    float max_y = -numeric_limits<float>::max();
    float max_z = -numeric_limits<float>::max();

    for(int i = 0; i < atom_list.size(); ++i)
    {
        min_x = min(min_x, atom_list[i].x);
        min_y = min(min_y, atom_list[i].y);
        min_z = min(min_z, atom_list[i].z);
        
        max_x = max(max_x, atom_list[i].x);
        max_y = max(max_y, atom_list[i].y);
        max_z = max(max_z, atom_list[i].z);
    }
    
    // This could overflow, but i doubt it 
    float ran_x = max_x - min_x;
    float ran_y = max_y - min_y;
    float ran_z = max_z - min_z;
    
    // Axis-aligned quantisation levels, should only be [1, 21] (TODO: at most 10 for now)
    int x_steps = 1 << x_step_bits;
    int y_steps = 1 << y_step_bits;
    int z_steps = 1 << z_step_bits;
    
    float x_block_size = ran_x/x_steps;
    float y_block_size = ran_y/y_steps;
    float z_block_size = ran_z/z_steps;
    
    // Place for the indices to go
    vector< pair<unsigned int, unsigned int> > indices;
    
    // Correct atoms to have non-negative coords, then quantise and index
    // From [min, max] to [0, ran] to [0, steps-1]
    for(int i = 0; i < atom_list.size(); ++i)
    {
        float sx, sy, sz;
        sx = atom_list[i].x - min_x;
        sy = atom_list[i].y - min_y;
        sz = atom_list[i].z - min_z;
        
        float nx, ny, nz;
        
        if(ran_x < 1e-5)
            nx = 0.0;
        else
            nx = ran_x - sx < 1e-5 ? x_steps-1 : int(sx/x_block_size);
        
        if(ran_y < 1e-5)
            ny = 0.0;
        else
            ny = ran_y - sy < 1e-5 ? y_steps-1 : int(sy/y_block_size);
        
        if(ran_z < 1e-5)
            nz = 0.0;
        else
            nz = ran_z - sz < 1e-5 ? z_steps-1 : int(sz/z_block_size);
        
        unsigned int ind = Atom(nx, ny, nz).get_index(x_step_bits, y_step_bits, z_step_bits); 
        indices.push_back(pair<unsigned int, unsigned int>(ind, i));
    }
    
    // Sort indices
    sort(indices.begin(), indices.end());
    
    // Each frame has a 288-byte header indicating index bits and bounding box.
    write_uint32(x_step_bits);
    write_uint32(y_step_bits);
    write_uint32(z_step_bits);
    write_uint32(*(unsigned int*)(&min_x));
    write_uint32(*(unsigned int*)(&min_y));
    write_uint32(*(unsigned int*)(&min_z));
    write_uint32(*(unsigned int*)(&max_x));
    write_uint32(*(unsigned int*)(&max_y));
    write_uint32(*(unsigned int*)(&max_z));
    
    OmelEncoder order_encoder(out_file, &bit_buffer);
    OmelEncoder dist_encoder(out_file, &bit_buffer);
    
    unsigned int lind = 0;
    unsigned int counts = 0;
    for(int i = 0; i < atoms; ++i)
        if(indices[i].second != order[i])
        {
            ++counts;
            
            order_encoder.write_uint32(i - lind);
            
            int t = indices[i].second;
            t -= order[i];
            
            dist_encoder.write_int32(t);
            order[i] = indices[i].second;
            lind = i;
        }
    order_encoder.write_uint32(atoms - lind);
            
    // Write the data
    octree_index_encoder.write_uint32(indices[0].first);
    
    for(int i = 1; i < atoms; ++i)
    {
        octree_index_encoder.write_uint32(indices[i].first - indices[i-1].first);
    }
    
    return true;
}

void OmelWriter::write_uint32(unsigned int num)
{
    // Write bits to our buffer
    for(int i = 0; i < 32; ++i)
        put_bit(num&(1<<i));
}

bool OmelWriter::put_bit(bool bit)
{
    bit_buffer.push(bit);
    
    if(bit_buffer.size() >= 1024)
        flush();
}

void OmelWriter::sort_indices(vector<unsigned int>& indices)
{
    if(indices.size() < 125000)
    {
        sort(indices.begin(), indices.end());
    }
    else
    {
        if(sort_num_buckets != indices.size())
        {
            // Create buckets
            if(sort_buckets != NULL)
                delete [] sort_buckets;
            
            sort_num_buckets = indices.size();
            sort_buckets = new vector<unsigned int>[sort_num_buckets];
        }
        
        unsigned int min_Ri = *min_element(indices.begin(), indices.end());
        unsigned int max_Ri = *max_element(indices.begin(), indices.end());
        
        unsigned int delta = max_Ri - min_Ri;
        unsigned int num_buckets = min(delta + 1, (unsigned int)indices.size());
        
        // Bucket atoms
        for(int i = 0; i < indices.size(); ++i)
        {
            int bucket = int(double(indices[i] - min_Ri)*(num_buckets-1)/double(delta));
            sort_buckets[bucket].push_back(indices[i]);
        }
        
        int replaced = 0;
        
        // Heap sort each bucket and merge them into indices array
        for(int i = 0; i < num_buckets; ++i)
        {
            make_heap(sort_buckets[i].begin(), sort_buckets[i].end());
            sort_heap(sort_buckets[i].begin(), sort_buckets[i].end());
            copy(sort_buckets[i].begin(), sort_buckets[i].end(), indices.begin()+replaced);
            replaced += sort_buckets[i].size();
            sort_buckets[i].clear();
        }
    }
}
