#include "OmelReader.h"
#include <vector>
#include <string>

using namespace std;

OmelReader::OmelReader(const char* filename, int in_initial_l, int in_delta_l, int in_max_adapt_initial_l, int in_max_adapt_delta_l)
{
    // Set compression variables
    initial_l = in_initial_l;
    delta_l = in_delta_l;
    max_adapt_initial_l = in_max_adapt_initial_l;
    max_adapt_delta_l = in_max_adapt_delta_l;
    adapt_initial_l = 0;
    adapt_delta_l = 0;
    
    // Set frame variables
    frames_left = 0;
    atoms = 0;
    
    istart = 0;
    nsavc = 1;
    delta = 1.0;
    
    // Open file
    in_file = fopen(filename, "rb");
}

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
    int x_step_bits = read_uncompressed_int();
    int y_step_bits = read_uncompressed_int();
    int z_step_bits = read_uncompressed_int();
    
    float min_x, min_y, min_z, max_x, max_y, max_z;
    
    // Read the bounding box data
    unsigned int tmp_read = 0;// read_uncompressed_int();
    tmp_read = read_uncompressed_int();
    min_x = *((float*)(&tmp_read));
    tmp_read = read_uncompressed_int();
    min_y = *((float*)(&tmp_read));
    tmp_read = read_uncompressed_int();
    min_z = *((float*)(&tmp_read));
    
    tmp_read = read_uncompressed_int();
    max_x = *((float*)(&tmp_read));
    tmp_read = read_uncompressed_int();
    max_y = *((float*)(&tmp_read));
    tmp_read = read_uncompressed_int();
    max_z = *((float*)(&tmp_read));
    
    unsigned int last = read_compressed_int();
    
    atom_list.push_back(Atom::create_from_index(last, x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z));
    
    for(int i = 1; i < atoms; ++i)
    {
        last += read_compressed_int();
    
        atom_list.push_back(Atom::create_from_index(last, x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z));
    }
    
    return true;
}

unsigned int OmelReader::read_uncompressed_int()
{
    // Read 32 bits from our buffer
    unsigned int tmp = 0;
 
    for(int i = 0; i < 32; ++i)
        tmp |= (get_bit()<<i);
    
    return tmp;
}

unsigned int OmelReader::read_compressed_int()
{
    unsigned int result = 0;
    int bits_read = 0;
    bool status;
    int alloc = initial_l;
    
    int num_statuses = 0;
    int adapt_initial_l_minus = 0;  // l being too long
    int adapt_initial_l_plus = 0;   // l being too short
    int adapt_delta_l_minus = 0;    // delta_l being too long
    int adapt_delta_l_plus = 0;     // delta_l being too short
        
    // Read data
    do
    {
        status = get_bit();
        
        for(int i = 0; i < alloc; ++i)
            result |= (get_bit()<<(bits_read++));
        
        alloc = delta_l;
        
        ++num_statuses;
    }
    while(status);
    
    if(num_statuses == 1)
    {
        // enough initial bits - adjust adapting information
        adapt_initial_l_minus = initial_l - num_bits(result);
    }
    else
    {
        // not enough initial bits - adjust adapting information
        adapt_initial_l_plus = num_statuses-1;
        adapt_delta_l_minus = bits_read - num_bits(result);
        adapt_delta_l_plus = num_statuses-2;
        
    }
        
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
    
    return result;
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
