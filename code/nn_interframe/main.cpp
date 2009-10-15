#include "pdbio/DCDWriter.h"
#include "pdbio/DCDReader.h"
#include "pdbio/PDBReader.h"
#include "NNInterframeWriter.h"
#include "NNInterframeReader.h"
#include "pdbio/AtomInformation.h"
#include "pdbio/Frame.h"
#include "quantiser/QuantisedFrame.h"
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <cassert>

using namespace std;

void print_usage()
{
    printf("Usage:\n");
    printf("Compression: driver c <input DCD file> <output compressed file>\n");
    printf("Decompression: driver x <input compressed file> <output DCD file>\n");
    printf("Switches: -x X-quantisation levels\n");
    printf("          -y Y-quantisation levels\n");
    printf("          -z Z-quantisation levels\n");
    printf("          -k prediction window size\n");
    printf("          -z vector size\n");
    
}

int main(int argc, char** argv)
{
    if(argc < 4)
    {
        print_usage();
        return 0;
    }

    unsigned int x_quant = 8;
    unsigned int y_quant = 8;
    unsigned int z_quant = 8;
    unsigned int k = 2;
    unsigned int v = 1;
    
    for(int i = 2; i < argc-1; ++i)
        if(strncmp(argv[i], "-x", 2) == 0)
            sscanf(argv[i+1], "%d", &x_quant);
        else if(strncmp(argv[i], "-y", 2) == 0)
            sscanf(argv[i+1], "%d", &y_quant);
        else if(strncmp(argv[i], "-z", 2) == 0)
            sscanf(argv[i+1], "%d", &z_quant);
        else if(strncmp(argv[i], "-k", 2) == 0)
            sscanf(argv[i+1], "%d", &k);
        else if(strncmp(argv[i], "-v", 2) == 0)
            sscanf(argv[i+1], "%d", &v);
        
    assert(k > v);

    if(argv[1][0] == 'c')
    {
        DCDReader dcdreader;
        dcdreader.open_file(argv[2]);
        Frame atoms(dcdreader.natoms());

        FILE* fout = fopen(argv[3], "w");
        NNInterframeWriter writer(fout, k, v);
        writer.start(dcdreader.natoms(), dcdreader.nframes()); 
        
        for(int i = 0 ; i < dcdreader.nframes(); ++i)
        {
            printf("\r");
            dcdreader.next_frame(atoms);
            QuantisedFrame qframe(atoms, x_quant, y_quant, z_quant);
            
            printf("Compressing: Frame %d written", i);
            fflush(stdout);
            writer.next_frame(qframe);
        }
        printf("\n");
        
        writer.end();
        fclose(fout);
    
    }
    else if(argv[1][0] == 'x')
    {   
        FILE* fin = fopen(argv[2], "r");
        
        NNInterframeReader reader(fin);
        reader.start();
        
        DCDWriter dcdwriter;
        dcdwriter.save_dcd_file(argv[3], reader.get_atoms());
            
        for(int i = reader.get_frames(); i > 0 ; --i)
        {
            printf("Uncompressing: %d frames left\r", i);
            fflush(stdout);
            QuantisedFrame qframe(reader.get_atoms(), x_quant, y_quant, z_quant);
            
            if(!reader.next_frame(qframe))
               break;

            Frame uncompressed = qframe.toFrame();
            dcdwriter.save_dcd_frame(uncompressed);
        }
        printf("\n");
        
        reader.end();
        fclose(fin);    
    }
    else
    {
        print_usage();
    }
}
