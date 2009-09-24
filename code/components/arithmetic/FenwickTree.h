#pragma once

class FenwickTree
{
    public:
        unsigned long long* array;
        unsigned int array_size;
        unsigned int base;
        
        unsigned int symbols;
        
        unsigned long long total_frequency;
        
        FenwickTree(unsigned int initial_length = 1000);
        ~FenwickTree();
        
        unsigned int add_symbol();
        
        void update(unsigned int index, unsigned long long amount = 1);
        
        unsigned long long query(unsigned int index);
        
        unsigned int cumulative_frequency(unsigned long long freq);
};