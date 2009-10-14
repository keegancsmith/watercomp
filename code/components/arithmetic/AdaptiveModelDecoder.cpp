#include "AdaptiveModelDecoder.h"
#include <string>
#include <cstdlib>
#include <cassert>

using namespace std;

AdaptiveModelDecoder::AdaptiveModelDecoder(ArithmeticDecoder* arithmetic_decoder)
{
    decoder = arithmetic_decoder;
    
    symbol_table.push_back("");
    symbol_table.push_back("_NEW");
    symbol_table.push_back("_EOF");
    
    tree.add_symbol();
    tree.add_symbol();
//     symbol_table[] = "_NEW";
//     symbol_table[tree.add_symbol()] = "_EOF";
    
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
        
        tree.add_symbol();
        symbol_table.push_back(string(new_symbol.begin(), new_symbol.end()));
        tree.update(symbol);
        
        return string(new_symbol.begin(), new_symbol.end());
    }
    else if(symbol_table[symbol] == "_EOF")
    {
        is_eof = true;
        return "_EOF";
    }
    
    tree.update(symbol);
    return symbol_table[symbol];
}

int AdaptiveModelDecoder::decode_int()
{
    string val = decode();
    assert(val.size() < 12);
    return atoi(val.c_str());
}
