#ifndef DCD_LOADER_H
#define DCD_LOADER_H

#include "dcdplugin.h"

// wrap up the dcd loader
class DCD_Loader
{
    public:
        DCD_Loader();
        ~DCD_Loader();

        int frame();
        void frame(int value);

        bool load_file(const char* filename);

    private:
        int _frame;
        int _numframes;
        int _natoms;

        dcdhandle* _dcd;
        molfile_timestep_t _dcd_frame_data;

        bool load_dcd_file(const char* filename);
        bool load_dcd_frame();
};//DCD_Loader

#endif
