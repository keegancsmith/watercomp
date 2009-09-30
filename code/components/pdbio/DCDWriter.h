#pragma once
//#include "../../dcd_loader/dcd.h"

#include "Frame.h"

class DCDWriter
{
    public:
        DCDWriter();
        ~DCDWriter();

        int natoms();

        bool save_dcd_file(const char* filename, int natoms);
        bool save_dcd_frame(Frame& data);

    private:
        int atoms;
        void* dcd;

};
