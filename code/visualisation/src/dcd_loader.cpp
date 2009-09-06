#include "dcd_loader.h"

#define ERROR fprintf

DCD_Loader::DCD_Loader()
{
    _frame = -1;
    _numframes = -1;
    _natoms = 0;

    _dcd = NULL;
    _dcd_frame_data.coords = NULL;
}//constructor

DCD_Loader::~DCD_Loader()
{
    if (_dcd != NULL)
        close_file_read(_dcd);
    if (_dcd_frame_data.coords != NULL)
        delete _dcd_frame_data.coords;
}//destructor


int DCD_Loader::frame()
{
    return _frame;
}//frame

void DCD_Loader::frame(int value)
{
    if (value < 0) value = 0;
    if (value >= _numframes) value = _numframes - 1;
    _frame = value;
}//frame


bool DCD_Loader::load_file(const char* filename)
{
    return load_dcd_file(filename);
}//load_file

bool DCD_Loader::load_dcd_file(const char* filename)
{
    void* v = open_dcd_read(filename, "dcd", &_natoms);
    if (!v)
    {
        ERROR(stderr, "DCD_Loader::load_dcd_file: error reading dcd file '%s'\n",
                filename);
        return false;
    }//if

    _dcd = (dcdhandle*)v;
    _frame = 0;
    _numframes = _dcd->nsets;
    _dcd_frame_data.coords = new float[3 * _natoms];
    return true;
}//load_dcd_file

bool DCD_Loader::load_dcd_frame()
{
    int rc = read_next_timestep(_dcd, _natoms, &_dcd_frame_data);
    if (rc)
    {
        ERROR(stderr, "DCD_Loader::load_dcd_frame: error reading frame data\n");
        return false;
    }//if
    return true;
}//load_dcd_frame

