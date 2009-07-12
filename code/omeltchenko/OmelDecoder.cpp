#include "OmelDecoder.h"
#include <queue>
#include <cstdio>

using namespace std;

OmelDecoder::OmelDecoder(FILE* out, queue<bool>* buffer, int in_initial_l, int in_delta_l, int in_max_adapt_initial_l, int in_max_adapt_delta_l)
{
    in_file = out;
    
    if(buffer == NULL)
    {
        delete_buffer = true;
        bit_buffer = new queue<bool>;
    }
    else
    {
        delete_buffer = false;
        bit_buffer = buffer;
    }
    
    // Set compression variables
    initial_l = in_initial_l;
    delta_l = in_delta_l;
    max_adapt_initial_l = in_max_adapt_initial_l;
    max_adapt_delta_l = in_max_adapt_delta_l;
    adapt_initial_l = 0;
    adapt_delta_l = 0;
}

OmelDecoder::~OmelDecoder()
{
    if(delete_buffer)
        delete bit_buffer;
    
    bit_buffer = NULL;
}

unsigned int OmelDecoder::read_uint32()
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

bool OmelDecoder::get_bit()
{
    if(bit_buffer->size() == 0)
    {
        unsigned char tmp;
        fread(&tmp, sizeof(unsigned char), 1, in_file);
        
        for(int i = 0; i < 8; ++i)
            bit_buffer->push(tmp&(1<<(7-i)));
    }
    
    bool ret_val = bit_buffer->front();
    bit_buffer->pop();
    
    return ret_val;
}

int OmelDecoder::num_bits(unsigned int num)
{
    int ans = 1; // 0 needs 1 bit to store
    
    while(num > 1)
    {
        ++ans;
        num>>=1;
    }
    
    return ans;
}