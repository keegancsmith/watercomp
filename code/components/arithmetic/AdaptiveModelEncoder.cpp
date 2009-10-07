#include "AdaptiveModelEncoder.h"
#include <string>
#include <cstdio>

using namespace std;

AdaptiveModelEncoder::AdaptiveModelEncoder(ArithmeticEncoder* arithmetic_encoder)
{
    encoder = arithmetic_encoder;
    
//     symbol_table["_NEW"] = tree.add_symbol();
//     symbol_table["_EOF"] = tree.add_symbol();
    symbol_table.insert("_NEW", tree.add_symbol());
    symbol_table.insert("_EOF", tree.add_symbol());
}
        
void AdaptiveModelEncoder::encode(string symbol)
{
//     printf("%s\n", symbol.c_str());
    
//     if(symbol_table.find(symbol) == symbol_table.end())
    if(!symbol_table.find_text(symbol))
    {
//         printf("NEW\n");
        encode("_NEW");
//         symbol_table[symbol] = tree.add_symbol();
        unsigned int new_s = tree.add_symbol();
        symbol_table.insert(symbol, new_s);
        
//         printf("AHHH %d %d\n", new_s, symbol_table.find_data(symbol));
        
        for(int i = 0; i < symbol.size(); ++i)
            encoder->encode(symbol[i], symbol[i]+1, 256);
        
        encoder->encode(0, 1, 256);
    }
    else
    {   
        unsigned int symbol_id = symbol_table.find_data(symbol);
//         printf("OLD %d %d\n", symbol_id, tree.symbols);
        encoder->encode(tree.query(symbol_id-1), tree.query(symbol_id), tree.total_frequency);
//         encoder->encode(tree.query(symbol_table[symbol]-1), tree.query(symbol_table[symbol]), tree.total_frequency);
        
//         printf("SD\n");
//         tree.update(symbol_table[symbol]);
        tree.update(symbol_id);
//         printf("DFG\n");
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
