#pragma once

#include <sys/types.h>
#include "Frame.h"

class DCDReader {
public:
    DCDReader();
    ~DCDReader();

    int frame() const;
    void set_frame(int frame);
    size_t nframes() const;
    int natoms() const;

    bool open_file(const char* filename);
    bool next_frame(Frame& frame);

private:
    void* _dcd;
    int _natoms;

    int _frame;
    int _nframes;
    int header_size;
    long long extrablocksize;
    long long ndims;
    long long firstframesize;
    long long framesize;

    //used in next_frame
    float unitcell[6];
};

