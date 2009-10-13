#include "FenwickTree.h"
#include <cstdlib>
#include <cstdio>
#include <algorithm>

FenwickTree::FenwickTree(unsigned int initial_length)
{
    array_size = initial_length;
    array = (unsigned long long*)calloc(array_size, sizeof(unsigned long long));
    symbols = 0;
    total_frequency = 0;
    base = 0;
}

FenwickTree::~FenwickTree()
{
    free(array);
}

unsigned int FenwickTree::add_symbol()
{
    /// Resize
    if(symbols+1 == array_size)
    {
        unsigned long long* tmp_array = (unsigned long long*)calloc(2*array_size, sizeof(unsigned long long));
        
        for(int i = 1; i < array_size; ++i)
            tmp_array[i] = query(i) - query(i-1);
        
        std::swap(tmp_array, array);
        array_size *= 2;
        
        total_frequency = 0;
        for(int i = array_size/2-1; i >= 1; --i)
        {
            unsigned long long value = array[i];
            array[i] = 0;
            update(i, value);
        }
        
        free(tmp_array);
    }
    
    unsigned int tmp = symbols+2;
//     printf("HURR %d %d\n", tmp, tmp&-tmp);
    
    if(tmp == (tmp & -tmp))
    {
        base = tmp;
    }
    
    update(symbols+1);
    return ++symbols;
}

void FenwickTree::update(unsigned int index, unsigned long long amount)
{
    if(index == 0)
        return;
    
    while(index < array_size)
    {
        array[index] += amount;
        index += (index & -index);
    }
    
    total_frequency += amount;
}

unsigned long long FenwickTree::query(unsigned int index)
{
    unsigned long long count = 0;
        
    while(index > 0)
    {
        count += array[index];
        index -= (index & -index);
    }
    
    return count;
}


unsigned int FenwickTree::cumulative_frequency(unsigned long long frequency)
{
    unsigned int index = 0;
    unsigned int mask = base;
    
    unsigned long long cum = 0;
    long long freq = frequency;
    
    while(mask && index <= symbols)
    {
        unsigned int midpt = index + mask;
        
        if(midpt < symbols && freq >= array[midpt])
        {
            index = midpt;
            freq -= array[midpt];
        }
        
        mask >>= 1U;
    }
    
    return index + 1;
    
    /**
    unsigned int start = 1;
    unsigned int end = symbols+1;
    
    if(frequency >= total_frequency || frequency < 0)
        return 0;
    
    while(end - start >= 1)
    {
        unsigned int midpt = (start + end)/2;
        unsigned long long freq_range_start = query(midpt-1);
        unsigned long long freq_range_end = query(midpt);
        
        if(frequency >= freq_range_start && frequency < freq_range_end)
        {
            return midpt;
        }
        if(frequency < freq_range_start)
            end = midpt;
        else if(frequency >= freq_range_end)
            start = midpt + 1;
    }
    
    return 0;
    **/
}