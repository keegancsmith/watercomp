#include "AdaptiveModelEncoder.h"
#include <string>
#include <cstdio>

using namespace std;

AdaptiveModelEncoder::AdaptiveModelEncoder(ArithmeticEncoder* arithmetic_encoder)
{
    encoder = arithmetic_encoder;
    
    symbol_table.insert("_NEW", 4, tree.add_symbol());
    symbol_table.insert("_EOF", 4, tree.add_symbol());
}

AdaptiveModelEncoder::~AdaptiveModelEncoder()
{
    symbol_table.destroy();
}
        
void AdaptiveModelEncoder::encode(string symbol)
{
    if(!symbol_table.find_text(symbol.c_str(), symbol.size()))
    {
        encode("_NEW");
        unsigned int new_s = tree.add_symbol();
        symbol_table.insert(symbol.c_str(), symbol.length(), new_s);
        
        for(size_t i = 0; i < symbol.size(); ++i)
            encoder->encode(symbol[i], symbol[i]+1, 256);
        
        encoder->encode(0, 1, 256);
    }
    else
    {   
        unsigned int symbol_id = symbol_table.find_data(symbol.c_str(), symbol.size());
        encoder->encode(tree.query(symbol_id-1), tree.query(symbol_id), tree.total_frequency);
        tree.update(symbol_id);
    }
}

void AdaptiveModelEncoder::encode_bytes(const void* symbol, int len)
{
    if(!symbol_table.find_text((char*)symbol, len))
    {
        encode("_NEW");
        unsigned int new_s = tree.add_symbol();
        symbol_table.insert((char*)symbol, len, new_s);
        
        for(int i = 0; i < len; ++i)
            encoder->encode(((unsigned char*)symbol)[i], ((unsigned char*)symbol)[i]+1, 257);
        
        encoder->encode(256, 257, 257);
    }
    else
    {   
        unsigned int symbol_id = symbol_table.find_data((char*)symbol, len);
        encoder->encode(tree.query(symbol_id-1), tree.query(symbol_id), tree.total_frequency);
        tree.update(symbol_id);
        
    }
}

void AdaptiveModelEncoder::encode_int(int i)
{
    encode_bytes(&i, sizeof(int));
}


void AdaptiveModelEncoder::end_encode()
{
    encode("_EOF");
}
