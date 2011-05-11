#include "pdbio/DCDWriter.h"
#include "pdbio/DCDReader.h"
#include "pdbio/PDBReader.h"
#include "LinearWriter.h"
#include "LinearReader.h"
#include "pdbio/AtomInformation.h"
#include "pdbio/Frame.h"
#include "quantiser/QuantisedFrame.h"
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <limits>

using namespace std;

void print_usage()
{
    printf("Usage:\n");
    printf("Compression: driver c <input DCD file> <output compressed file>\n");
    printf("Decompression: driver x <input compressed file> <output DCD file>\n");
    printf("Switches: -x X-quantisation levels\n");
    printf("          -y Y-quantisation levels\n");
    printf("          -z Z-quantisation levels\n");
    printf("          -o output error histogram to errors.csv\n");
}

int main(int argc, char** argv)
{    
    if(argc < 4)
    {
        print_usage();
        return 0;
    }

    bool output = false;
    
    unsigned int x_quant = 8;
    unsigned int y_quant = 8;
    unsigned int z_quant = 8;
    
    for(int i = 2; i < argc; ++i)
        if(strncmp(argv[i], "-x", 2) == 0)
            sscanf(argv[i+1], "%d", &x_quant);
        else if(strncmp(argv[i], "-y", 2) == 0)
            sscanf(argv[i+1], "%d", &y_quant);
        else if(strncmp(argv[i], "-z", 2) == 0)
            sscanf(argv[i+1], "%d", &z_quant);
        else if(strncmp(argv[i], "-o", 2) == 0)
            output = true;
        
        
    if(argv[1][0] == 'c')
    {
        float min_coord[3];
        float max_coord[3];
        
        for(int d = 0; d < 3; ++d)
            min_coord[d] = numeric_limits<float>::max();
    
        for(int d = 0; d < 3; ++d)
            max_coord[d] = numeric_limits<float>::min();
        
        DCDReader dcdreader;
        dcdreader.open_file(argv[2]);
        Frame atoms(dcdreader.natoms());

        FILE* fout = fopen(argv[3], "w");
        LinearWriter writer(fout, output);
        writer.start(dcdreader.natoms(), dcdreader.nframes()); 
        
        for(int i = 0 ; i < dcdreader.nframes(); ++i)
        {
            dcdreader.next_frame(atoms);
            QuantisedFrame qframe(atoms, x_quant, y_quant, z_quant, min_coord, max_coord);
            
            for(int d = 0; d < 3; ++d)
                min_coord[d] = qframe.min_coord[d];
            
            for(int d = 0; d < 3; ++d)
                max_coord[d] = qframe.max_coord[d];
            
            writer.next_frame(qframe);
        }
        
        writer.end();
        fclose(fout);
    
    }
    else if(argv[1][0] == 'x')
    {
        FILE* fin = fopen(argv[2], "r");
        
        LinearReader reader(fin);
        reader.start();
        
        DCDWriter dcdwriter;
        dcdwriter.save_dcd_file(argv[3], reader.get_atoms());
            
        for(int i = reader.get_frames(); i > 0 ; --i)
        {   
            QuantisedFrame qframe(reader.get_atoms(), 8, 8, 8);
            
            if(!reader.next_frame(qframe))
               break;

            Frame uncompressed = qframe.toFrame();
            dcdwriter.save_dcd_frame(uncompressed);
        }
        
        reader.end();
        fclose(fin);    
    }
    else
    {
        print_usage();
    }
}
