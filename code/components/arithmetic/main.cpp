#include <iostream>
#include <string>

#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"
#include "AdaptiveModelEncoder.h"
#include "AdaptiveModelDecoder.h"

using namespace std;

int main(int argc, char** argv)
{
    FILE* source = fopen(argv[1], "r");
    
    ArithmeticEncoder ae;
    FILE* out = fopen("test.out", "w");
    ae.start_encode(out);
    AdaptiveModelEncoder encoder(&ae);
    char t[100];
    
    int D = 0;
    int S = 0;
    fseek(source, 0, SEEK_END);
    S = ftell(source);
    fseek(source, 0, SEEK_SET);
    
    int last_percentage = -1;
    
    
    while(true)
    {
        unsigned char byte;
        fread(&byte, 1, 1, source);
        
        if(feof(source))
            break;
        
        sprintf(t, "%d", byte);
        encoder.encode(t);
        
        ++D;
        
        if(100*D/S != last_percentage)
        {
            last_percentage = 100*D/S;
            fprintf(stderr, "%d%% = %d %d\n", last_percentage, D, S); 
        }
    }
    
    encoder.end_encode();
    ae.end_encode();
    fclose(out);
    fclose(source);
    
    
    ArithmeticDecoder ad;
    FILE* in = fopen("test.out", "r");
    ad.start_decode(in);
    AdaptiveModelDecoder decoder(&ad);
    
    FILE* destination = fopen(argv[2], "w");
    
    int h = 0;
    
    while(true)
    {
        string s = decoder.decode();
        
        int val;
        sscanf(s.c_str(), "%d", &val);
        
        if(s == "_EOF")
            break;
        
        fprintf(destination, "%c", val);
    }
    
    fclose(destination);
    fclose(in);
    
    return 0;
}
