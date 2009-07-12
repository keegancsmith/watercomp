#include <cstdio>
#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
#include "Atom.h"
#include "OmelWriter.h"
#include <string>
using namespace std;

OmelWriter::OmelWriter(const char* filename, int in_initial_l, int in_delta_l, int in_max_adapt_initial_l, int in_max_adapt_delta_l)
{
    // Set compression variables
    initial_l = in_initial_l;
    delta_l = in_delta_l;
    max_adapt_initial_l = in_max_adapt_initial_l;
    max_adapt_delta_l = in_max_adapt_delta_l;
    adapt_initial_l = 0;
    adapt_delta_l = 0;
    
    // Set frame variables
    frames_written = 0;
    atoms = 0;
    
    // Open file
    out_file = fopen(filename, "wb");
    
    // Bucket sorting variables
    sort_num_buckets = 0;
    sort_buckets = NULL;
}

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
    vector<unsigned int> indices;
    
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
        
        indices.push_back(Atom(nx, ny, nz).get_index(x_step_bits, y_step_bits, z_step_bits));
    }
    
    // Sort indices
    sort(indices.begin(), indices.end());
    
    // Each frame has a 288-byte header indicating index bits and bounding box.
    write_uncompressed_int(x_step_bits);
    write_uncompressed_int(y_step_bits);
    write_uncompressed_int(z_step_bits);
    write_uncompressed_int(*(unsigned int*)(&min_x));
    write_uncompressed_int(*(unsigned int*)(&min_y));
    write_uncompressed_int(*(unsigned int*)(&min_z));
    write_uncompressed_int(*(unsigned int*)(&max_x));
    write_uncompressed_int(*(unsigned int*)(&max_y));
    write_uncompressed_int(*(unsigned int*)(&max_z));
        
    // Write the data
    unsigned int last = indices[0];
    write_compressed_int(last);
    
    for(int i = 1; i < atoms; ++i)
    {
        unsigned int next = indices[i];
        write_compressed_int(next - last);
        last = next;
    }
    
    return true;
}

void OmelWriter::write_uncompressed_int(unsigned int num)
{
    // Write bits to our buffer
    for(int i = 0; i < 32; ++i)
        put_bit(num&(1<<i));
}

void OmelWriter::write_compressed_int(unsigned int num)
{
    unsigned int tt = num;
    int li = num_bits(num);
    if(li == 0)
        li = 1;
    
    int alloc = initial_l;
    
    int adapt_initial_l_minus = 0;  // l being too long
    int adapt_initial_l_plus = 0;   // l being too short
    int adapt_delta_l_minus = 0;    // delta_l being too long
    int adapt_delta_l_plus = 0;     // delta_l being too short
    
    if(li <= initial_l)
    {
        // enough initial bits - adjust adapting information
        adapt_initial_l_minus = initial_l - li;
    }
    else
    {
        // not enough initial bits - adjust adapting information
        adapt_initial_l_plus = (li - initial_l + delta_l - 1)/delta_l;
        adapt_delta_l_plus = adapt_initial_l_plus-1;
        adapt_delta_l_minus = (delta_l - (li - initial_l)%delta_l)%delta_l; // Number of padding bits
    }
    
    do
    {
        // Output status bit: 0 = last allocation of bits, 1 = more allocated bits
        put_bit(li > alloc);
        
        // Write bit data
        for(int i = 0; i < alloc; ++i)
        {
            put_bit(num&1);
            num >>= 1;
        }
        
        li -= alloc;
        alloc = delta_l;
        
    }
    while(li > 0);
    
    // Update the adaptive variables
    adapt_initial_l += (adapt_initial_l_plus - adapt_initial_l_minus);
    adapt_delta_l += (adapt_delta_l_plus - adapt_delta_l_minus);
    
    // Potentially change allocations based on adaptive variables
    if(adapt_initial_l >= max_adapt_initial_l)
    {
        ++initial_l;
        adapt_initial_l = 0;
    }
    else if(adapt_initial_l <= -max_adapt_initial_l)
    {
        --initial_l;
        adapt_initial_l = 0;
        
        if(initial_l < 1)
            initial_l = 1;
    }
    
    if(adapt_delta_l >= max_adapt_delta_l)
    {
        ++delta_l;
        adapt_delta_l = 0;
    }
    else if(adapt_delta_l <= -max_adapt_delta_l)
    {
        --delta_l;
        adapt_delta_l = 0;
        
        if(delta_l < 1)
            delta_l = 1;
    }
}

bool OmelWriter::put_bit(bool bit)
{
    bit_buffer.push(bit);
    
    if(bit_buffer.size() >= 1024)
        flush();
}

int OmelWriter::num_bits(unsigned int num)
{
    int ans = 1; // 0 needs 1 bit to store
    
    while(num > 1)
    {
        ++ans;
        num>>=1;
    }
    
    return ans;
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