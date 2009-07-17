#include "OmelReader.h"
#include <vector>
#include <string>

using namespace std;


void OmelReader::start_read()
{
    // Reads the number of atoms and the number of frames
    // Expects there to be same number of atoms in each frame.
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&frames_left, sizeof(unsigned int), 1, in_file);
    fread(&istart, sizeof(int), 1, in_file);
    fread(&nsavc, sizeof(int), 1, in_file);
    fread(&delta, sizeof(double), 1, in_file);
}

void OmelReader::end_read()
{
    fclose(in_file);
}

bool OmelReader::read_frame(vector<Atom>& atom_list)
{
    if(frames_left == 0)
        return false;
    
    --frames_left;
    
    // Read the index specification data
    int x_step_bits = read_uint32();
    int y_step_bits = read_uint32();
    int z_step_bits = read_uint32();
    
    float min_x, min_y, min_z, max_x, max_y, max_z;
    
    // Read the bounding box data
    unsigned int tmp_read = 0;// read_uncompressed_int();
    tmp_read = read_uint32();
    min_x = *((float*)(&tmp_read));
    tmp_read = read_uint32();
    min_y = *((float*)(&tmp_read));
    tmp_read = read_uint32();
    min_z = *((float*)(&tmp_read));
    
    tmp_read = read_uint32();
    max_x = *((float*)(&tmp_read));
    tmp_read = read_uint32();
    max_y = *((float*)(&tmp_read));
    tmp_read = read_uint32();
    max_z = *((float*)(&tmp_read));
    
    atom_list.resize(atoms);
    unsigned int last = octree_index_decoder.read_uint32();
    unsigned int ind = array_index_decoder.read_uint32();
    
    atom_list[ind] = Atom::create_from_index(last, x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z);

    for(int i = 1; i < atoms; ++i)
    {
        last += octree_index_decoder.read_uint32();
        ind = array_index_decoder.read_uint32();
        atom_list[ind] = Atom::create_from_index(last, x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z);
    }
    
    return true;
}

unsigned int OmelReader::read_uint32()
{
    // Read 32 bits from our buffer
    unsigned int tmp = 0;
 
    for(int i = 0; i < 32; ++i)
        tmp |= (get_bit()<<i);
    
    return tmp;
}

bool OmelReader::get_bit()
{
    if(bit_buffer.size() == 0)
    {
        unsigned char tmp;
        fread(&tmp, sizeof(unsigned char), 1, in_file);
        
        for(int i = 0; i < 8; ++i)
            bit_buffer.push(tmp&(1<<(7-i)));
    }
    
    bool ret_val = bit_buffer.front();
    bit_buffer.pop();
    
    return ret_val;
}

int OmelReader::num_bits(unsigned int num)
{
    int ans = 1; // 0 is 1 bit long
    
    while(num > 1)
    {
        ++ans;
        num>>=1;
    }
    
    return ans;
}
