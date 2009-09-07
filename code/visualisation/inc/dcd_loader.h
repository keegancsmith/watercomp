#ifndef DCD_LOADER_H
#define DCD_LOADER_H

class Frame_Data;

// wrap up the dcd loader
class DCD_Loader
{
    public:
        DCD_Loader();
        ~DCD_Loader();

        int frame();
        bool frame(int value);
        int totalFrames();

        bool load_dcd_file(const char* filename);
        bool load_dcd_frame(Frame_Data* data);

    private:
        int _frame;
        int _numframes;
        int _natoms;
        int header_size;
        long long extrablocksize;
        long long ndims;
        long long firstframesize;
        long long framesize;

        void* _dcd;

};//DCD_Loader

#endif
