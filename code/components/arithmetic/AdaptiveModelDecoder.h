#pragma once
#include "ArithmeticDecoder.h"
#include "FenwickTree.h"
#include <map>
#include <string>
#include <vector>

/**
 A Model represents a way to map from data 
 received from the Arithmetic Coder and actual
 symbols.

 Each model must have three things:
  Mapping from arithmetic decoder bucket to symbol
  Mapping from symbol to range for arithmetic encoder
  Current total frequency
**/

class AdaptiveModelDecoder
{
    public:
        FenwickTree tree;
        
        ArithmeticDecoder* decoder;

        std::vector<char*> symbol_table;
        std::vector<unsigned int> symbol_table_sizes;
        bool is_eof;
        
        AdaptiveModelDecoder(ArithmeticDecoder* arithmetic_decoder);
        ~AdaptiveModelDecoder();
        
        std::string decode();
        int decode_int();
        void decode_bytes(void* buffer);
};
