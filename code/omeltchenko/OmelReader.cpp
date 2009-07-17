#include "OmelReader.h"
#include <vector>
#include <string>

using namespace std;


void OmelReader::start_read()
{
    // Reads the number of atoms and the number of frames
    // Expects there to be same number of atoms in each frame.
    fread(&atoms, sizeof(unsigned int), 1, in_file);
    fread(&total_frames, sizeof(unsigned int), 1, in_file);
    fread(&p_frames, sizeof(unsigned int), 1, in_file);
    
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
//     printf("%d %d %d\n", frames_read, total_frames, frames_read-total_frames);
    if(frames_read == total_frames)
        return false;
    
    if(order.size() != atoms)
        for(int i = 0; i < atoms; ++i)
            order.push_back(i);
    
    last_frame.swap(second_last_frame);
    if(last_frame.size() != atoms)
        last_frame.resize(atoms);
    
    if(second_last_frame.size() != atoms)
        second_last_frame.resize(atoms);
    
    // Read the index specification data
    int x_step_bits = read_uint32();
    int y_step_bits = read_uint32();
    int z_step_bits = read_uint32();
    
    float min_x, min_y, min_z, max_x, max_y, max_z;
    
    // Read the bounding box data
    unsigned int tmp_read = 0;
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
    
    if(frames_read % p_frames == 0)
    {
        // Index Frame
        
        OmelDecoder order_decoder(in_file, &bit_buffer);
        OmelDecoder dist_decoder(in_file, &bit_buffer);
        
        unsigned int lind = 0;
        
        while(true)
        {
            unsigned int index = lind + order_decoder.read_uint32();
            
            if(index == atoms)
                break;
            
            int changed = dist_decoder.read_int32();
            order[index] += changed;
            lind = index;
        }
        
        atom_list.resize(atoms);
        unsigned int last = octree_index_decoder.read_uint32();
        
        last_frame[order[0]] = last;
        atom_list[order[0]] = Atom::create_from_index(last, x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z);

        for(int i = 1; i < atoms; ++i)
        {
            last += octree_index_decoder.read_uint32();
            
            last_frame[order[i]] = last;

            atom_list[order[i]] = Atom::create_from_index(last, x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z);
        }
    }
    else if(frames_read % p_frames == 1)
    {

        // Only error in position
        atom_list.resize(atoms);
        
//         char buffy[1000];
//         sprintf(buffy, "PRead.txt", frames_read);
//         FILE* ftmp = fopen(buffy, "w");
        
        for(int i = 0; i < atoms; ++i)
        {
            last_frame[i] = second_last_frame[i];
            last_frame[i] += pframe_decoder.read_int32();
            
//             fprintf(ftmp, "%d %u %u %d\n", i, second_last_frame[i], last_frame[i], eee);
            
            atom_list[i] = Atom::create_from_index(last_frame[i], x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z);
        }
        
//         fclose(ftmp);
    }
    else
    {
        // Error in position predicted by velocity.
        
        atom_list.resize(atoms);
        
//         char buffy[1000];
//         sprintf(buffy, "P Frame %d Read.txt", frames_read);
//         FILE* ftmp;
//         
//         if(frames_read == 3)
//             ftmp = fopen(buffy, "w");
        
        for(int i = 0; i < atoms; ++i)
        {
            int delta = second_last_frame[i];
            delta -= last_frame[i];
            
            // D = S - L
            // E = S + D - I
            
            int error = pframe_decoder.read_int32();
            
//             if(frames_read == 3)
//                 fprintf(ftmp, "%d %u %u %u %d %d %d\n", i, second_last_frame[i] - error + delta, second_last_frame[i], last_frame[i], error-delta, error, delta);
            
            last_frame[i] = second_last_frame[i] + delta - error;
            
            
            atom_list[i] = Atom::create_from_index(last_frame[i], x_step_bits, y_step_bits, z_step_bits, min_x, min_y, min_z, max_x, max_y, max_z);
        }
        
//         if(frames_read == 3)
//             fclose(ftmp);
    }
    
    ++frames_read;  
    
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
