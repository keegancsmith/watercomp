#pragma once
#include "ArithmeticEncoder.h"
#include "FenwickTree.h"
#include <map>
#include <string>

/**
 A Model represents a way to map from data 
 received from the Arithmetic Coder and actual
 symbols.

 Each model must have three things:
  Mapping from arithmetic decoder bucket to symbol
  Mapping from symbol to range for arithmetic encoder
  Current total frequency
**/

class AdaptiveModelEncoder
{
    public:
        FenwickTree tree;
        
        ArithmeticEncoder* encoder;
        
        std::map<std::string, unsigned int> symbol_table;
        
        AdaptiveModelEncoder(ArithmeticEncoder* arithmetic_encoder);
        
        void encode(std::string symbol);
        void end_encode();

};