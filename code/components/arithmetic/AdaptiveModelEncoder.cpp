#include "AdaptiveModelEncoder.h"
#include <string>
#include <cstdio>

using namespace std;

AdaptiveModelEncoder::AdaptiveModelEncoder(ArithmeticEncoder* arithmetic_encoder)
{
    encoder = arithmetic_encoder;
    
    symbol_table.insert("_NEW", tree.add_symbol());
    symbol_table.insert("_EOF", tree.add_symbol());
}
        
void AdaptiveModelEncoder::encode(string symbol)
{
    if(!symbol_table.find_text(symbol))
    {
        encode("_NEW");
        unsigned int new_s = tree.add_symbol();
        symbol_table.insert(symbol, new_s);
        
        for(int i = 0; i < symbol.size(); ++i)
            encoder->encode(symbol[i], symbol[i]+1, 256);
        
        encoder->encode(0, 1, 256);
    }
    else
    {   
        unsigned int symbol_id = symbol_table.find_data(symbol);
        encoder->encode(tree.query(symbol_id-1), tree.query(symbol_id), tree.total_frequency);
        tree.update(symbol_id);
    }
}

void AdaptiveModelEncoder::encode_int(int i)
{
    char buf[12]; // Ints can only be 10 digits + sign + null
    sprintf(buf, "%d", i);
    encode(buf);
}


void AdaptiveModelEncoder::end_encode()
{
    encode("_EOF");
}
