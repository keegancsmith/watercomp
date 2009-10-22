#include "AdaptiveModelDecoder.h"
#include <string>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <algorithm>

using namespace std;

AdaptiveModelDecoder::AdaptiveModelDecoder(ArithmeticDecoder* arithmetic_decoder)
{
    decoder = arithmetic_decoder;
    
    symbol_table.push_back(NULL);
    symbol_table.push_back(new char[4]);
    symbol_table[symbol_table.size()-1][0] = '_';
    symbol_table[symbol_table.size()-1][1] = 'N';
    symbol_table[symbol_table.size()-1][2] = 'E';
    symbol_table[symbol_table.size()-1][3] = 'W';
    
    symbol_table.push_back(new char[4]);
    symbol_table[symbol_table.size()-1][0] = '_';
    symbol_table[symbol_table.size()-1][1] = 'E';
    symbol_table[symbol_table.size()-1][2] = 'O';
    symbol_table[symbol_table.size()-1][3] = 'F';
    
    symbol_table_sizes.push_back(0);
    symbol_table_sizes.push_back(4);
    symbol_table_sizes.push_back(4);
    
    tree.add_symbol();
    tree.add_symbol();
    
    is_eof = false;
}

AdaptiveModelDecoder::~AdaptiveModelDecoder()
{
    for(int i = 1; i < symbol_table.size(); ++i)
        delete [] symbol_table[i];
}

string AdaptiveModelDecoder::decode()
{
    if(is_eof)
        return "";
    
    unsigned long long frequency = decoder->decode(tree.total_frequency);
    unsigned int symbol = tree.cumulative_frequency(frequency);
    decoder->decoder_update(tree.query(symbol - 1), tree.query(symbol));
    
    if(strncmp(symbol_table[symbol], "_NEW", 4) == 0)
    {
        vector<char> new_symbol;
        int chr;
        
        do
        {
            chr = decoder->decode(256);
            new_symbol.push_back(chr);
            decoder->decoder_update(chr, chr+1);
        }
        while(chr != 0);
        
        tree.add_symbol();
        symbol_table.push_back(new char[new_symbol.size()]);
        copy(new_symbol.begin(), new_symbol.end(), symbol_table[symbol_table.size()-1]);
        symbol_table_sizes.push_back(new_symbol.size()-1);
        tree.update(symbol);
        
        return symbol_table[symbol_table.size()-1];
    }
    else if(strncmp(symbol_table[symbol], "_EOF", 4) == 0)
    {
        is_eof = true;
        return "";
    }
    
    tree.update(symbol);
    return symbol_table[symbol];
}

int AdaptiveModelDecoder::decode_int()
{
    int i;
    decode_bytes(&i);
    return i;
}

void AdaptiveModelDecoder::decode_bytes(void* buffer)
{
    if(is_eof)
    {
        *(char*)buffer = '_';
        *(char*)buffer = 'E';
        *(char*)buffer = 'O';
        *(char*)buffer = 'F';
        return;
    }
    
    unsigned long long frequency = decoder->decode(tree.total_frequency);
    unsigned int symbol = tree.cumulative_frequency(frequency);
    decoder->decoder_update(tree.query(symbol - 1), tree.query(symbol));
    
    if(strncmp(symbol_table[symbol], "_NEW", 4) == 0)
    {
        vector<char> new_symbol;
        int chr;
        
        do
        {
            chr = decoder->decode(257);
            new_symbol.push_back(chr);
            decoder->decoder_update(chr, chr+1);
        }
        while(chr != 256);
        
        tree.add_symbol();
        symbol_table.push_back(new char[new_symbol.size()]);
        copy(new_symbol.begin(), new_symbol.end(), symbol_table[symbol_table.size()-1]);
        symbol_table_sizes.push_back(new_symbol.size()-1);
        tree.update(symbol);
        
        copy(symbol_table[symbol_table.size()-1], symbol_table[symbol_table.size()-1] + symbol_table_sizes[symbol_table.size()-1], (char*)buffer);
        
        return;
    }
    else if(strncmp(symbol_table[symbol], "_EOF", 4) == 0)
    {
        is_eof = true;
        *(char*)buffer = '_';
        *(char*)buffer = 'E';
        *(char*)buffer = 'O';
        *(char*)buffer = 'F';
        return;
    }
    
    tree.update(symbol);
    copy(symbol_table[symbol], symbol_table[symbol] + symbol_table_sizes[symbol], (char*)buffer);
    return;
}
