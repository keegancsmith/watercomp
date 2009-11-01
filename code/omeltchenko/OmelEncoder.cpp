#include <queue>
#include <cstdio>
#include "OmelEncoder.h"

using namespace std;

OmelEncoder::OmelEncoder(FILE* out, queue<bool>* buffer, int in_initial_l, int in_delta_l, int in_max_adapt_initial_l, int in_max_adapt_delta_l)
{
    out_file = out;
    
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

OmelEncoder::~OmelEncoder()
{
    if(delete_buffer)
        delete bit_buffer;
    
    bit_buffer = NULL;
}

void OmelEncoder::write_uint64(unsigned long long num)
{
    int li = num_bits(num);
    
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
            put_bit(num&1ULL);
            num >>= 1ULL;
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




bool OmelEncoder::put_bit(bool bit)
{
    bit_buffer->push(bit);
    
    if(bit_buffer->size() >= 1024)
        flush();
}

void OmelEncoder::flush()
{
    // Flush allfull bytes out to the file
    while(bit_buffer->size() >= 8)
    {
        unsigned char acc = 0;
        for(int i = 0; i < 8; ++i)
        {
            acc = (acc << 1) | bit_buffer->front();
            bit_buffer->pop();
        }
        fprintf(out_file, "%c", acc);
    }
}

int OmelEncoder::num_bits(unsigned long long num)
{
    int ans = 1; // 0 needs 1 bit to store
    
    while(num > 1)
    {
        ++ans;
        num>>=1ULL;
    }
    
    return ans;
}
