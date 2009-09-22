#include "AdaptiveModelDecoder.h"
#include <string>

using namespace std;

AdaptiveModelDecoder::AdaptiveModelDecoder(ArithmeticDecoder* arithmetic_decoder)
{
    decoder = arithmetic_decoder;
    
    symbol_table[tree.add_symbol()] = "_NEW";
    symbol_table[tree.add_symbol()] = "_EOF";
    
    is_eof = false;
}

string AdaptiveModelDecoder::decode()
{
    if(is_eof)
        return "";
    
    unsigned long long frequency = decoder->decode(tree.total_frequency);
    unsigned int symbol = tree.cumulative_frequency(frequency);
    decoder->decoder_update(tree.query(symbol - 1), tree.query(symbol));
    
    if(symbol_table[symbol] == "_NEW")
    {
        vector<char> new_symbol;
        unsigned int chr;
        
        do
        {
            chr = decoder->decode(256);
            new_symbol.push_back(chr);
            decoder->decoder_update(chr, chr+1);
        }
        while(chr != 0);
        
        symbol_table[tree.add_symbol()] = string(new_symbol.begin(), new_symbol.end());
        
        tree.update(symbol);
        return string(new_symbol.begin(), new_symbol.end());
        
//         tree.add_symbol();



//         frequencies.add_symbol();
//         vector<char> new_symbol;
//         unsigned int chr;
//         
//         do
//         {
//             chr = decode(256);
// //             printf("I %c %d\n", chr, chr);
//             decoder_update(chr, chr+1);
//             
//             new_symbol.push_back(chr);
//         }
//         while(chr != 0);
        
//         symbol_map[frequencies.add_symbol()] = string(new_symbol.begin(), new_symbol.end());
        
//         printf("New symbol: %s\n", string(new_symbol.begin(), new_symbol.end()).c_str());
    }
    else if(symbol_table[symbol] == "_EOF")
    {
        is_eof = true;
        return "_EOF";
    }
    
    tree.update(symbol);
    
    return symbol_table[symbol];
}