#define FILE_OFFSET_BITS 64

#include <cstdio>
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
    
    long long a = 0;
    while(true)
    {
        unsigned char byte;
        fread(&byte, 1, 1, source);
        
//         printf("%d %lld %llX %llX %u\n", byte, a++, ae.encoder_low, ae.encoder_high, ae.e3_scaling);
        
        if((a++ & 1048575) == 0)
            printf("%lld\n", a>>20LL);
        if(feof(source))
            break;
        
        sprintf(t, "%d", byte);
        encoder.encode(t);
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