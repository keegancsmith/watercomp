#include "../dcd_loader/dcdplugin.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <cassert>
#include <limits>
#include <string>
#include "Atom.h"
#include "OmelReader.h"
#include "OmelWriter.h"

using namespace std;

void print_usage()
{
    printf("USAGE: ./driver [c|x] [INPUT FILE] [OUTPUT FILE]\n");
    printf("Compresses(c) or uncompresses(x) a file using the Omeltchenko reference scheme.\n");
    printf("When compressing output file is [FILE].cmp\n");
    printf("When uncompressing output file is [FILE].dcd\n");
    
}

int main(int argc, char** argv)
{
    if(argc != 3 && argc != 4)
    {
        print_usage();
        return 0;
    }
    
    string method = argv[1];
    
    if(method == "c")
    {
        string in_file = string(argv[2]);
        string out_file = in_file + ".cmp";
        
        if(argc == 4)
            out_file = string(argv[3]);
        
        if(in_file == out_file)
        {
            printf("Error: Input File = Output File\n");
            exit(1);
        }
        
        OmelWriter writer(out_file.c_str());
        int natoms = 0;
        
        dcdhandle* dcd = (dcdhandle*) open_dcd_read(in_file.c_str(), "dcd", &natoms);
        molfile_timestep_t timestep;
        timestep.coords = new float[3*natoms];
        
        writer.start_write(dcd->istart, dcd->nsavc, dcd->delta);
        
        // Read atoms and compress
        for(int i = 0; i < dcd->nsets; ++i)
        {
            printf("Processing Frame %d\n", i+1);
            
            if(read_next_timestep(dcd, natoms, &timestep)) 
            {
                fprintf(stderr, "error in read_next_timestep on frame %d\n", i);
                return 1;
            }
            
            vector<Atom> frame;
            
            for(int j = 0; j < natoms; ++j)
                frame.push_back(Atom(timestep.coords[3*j], timestep.coords[1+3*j], timestep.coords[2+3*j]));
            
            writer.write_frame(frame, 10, 10, 10);
        }
        
        writer.end_write();
    }
    else if(method == "x")
    {
        string in_file = string(argv[2]);
        string out_file = in_file + ".dcd";
        
        if(argc == 4)
            out_file = string(argv[3]);
        
        if(in_file == out_file)
        {
            printf("Error: Input File = Output File\n");
            exit(1);
        }
        
        // Open compressed file for reading
        OmelReader reader(in_file.c_str());
        
        // Reads all the necessary information for creating a dcd headers
        reader.start_read(); 
        
        // Open dcd file for writing
        fio_fd fd;
        
        if (fio_open(out_file.c_str(), FIO_WRITE, &fd) < 0) 
        {
            printf("Could not open file '%s' for writing\n", out_file.c_str());
            return 1;
        }

        write_dcdheader(fd, "Created by Omelchenko Compressor by VMD plugin.", reader.get_atoms(), reader.get_istart(), reader.get_nsavc(), reader.get_delta(), 0, 1);
        
        // This is a fail way, it involves copying data again, can fix this later.
        float* xs = new float[1*reader.get_atoms()];
        float* ys = new float[1*reader.get_atoms()];
        float* zs = new float[1*reader.get_atoms()];
        vector<Atom> frame;
        
        int frame_no = 0;
        
        while(reader.read_frame(frame))
        {
            ++frame_no;
            printf("Processing Frame %d\n", frame_no);
            // Read compressed atoms
            for(int i = 0; i < frame.size(); ++i)
            {
                xs[i] = frame[i].x;
                ys[i] = frame[i].y;
                zs[i] = frame[i].z;
            }
            frame.clear();
            
            // Write uncompressed atoms
            write_dcdstep(fd, frame_no, reader.get_istart() + reader.get_nsavc()*frame_no, reader.get_atoms(), xs, ys, zs, NULL, 1);
            
            printf("End processing\n");
        }
        
        fio_fclose(fd);
        delete [] xs;
        delete [] ys;
        delete [] zs;
        
        reader.end_read();
    }
    else
    {
        print_usage();
        return 0;
    }
}
