#include "AdaptiveModelEncoder.h"
#include <string>

using namespace std;

AdaptiveModelEncoder::AdaptiveModelEncoder(ArithmeticEncoder* arithmetic_encoder)
{
    encoder = arithmetic_encoder;
    
    symbol_table["_NEW"] = tree.add_symbol();
    symbol_table["_EOF"] = tree.add_symbol();
}
        
void AdaptiveModelEncoder::encode(string symbol)
{
    if(symbol_table.find(symbol) == symbol_table.end())
    {
        encode("_NEW");
        symbol_table[symbol] = tree.add_symbol();
        
        for(int i = 0; i < symbol.size(); ++i)
        {
//             printf("%d %c to %d %c\n", symbol[i], symbol[i], symbol[i]+1, symbol[i]+1); 
            encoder->encode(symbol[i], symbol[i]+1, 256);
        }
        encoder->encode(0, 1, 256);
        
//         printf("NEW SYMBOL END\n");
    }
    else
    {   
        encoder->encode(tree.query(symbol_table[symbol]-1), tree.query(symbol_table[symbol]), tree.total_frequency);
        tree.update(symbol_table[symbol]);
    }
}

void AdaptiveModelEncoder::end_encode()
{
    encode("_EOF");
}