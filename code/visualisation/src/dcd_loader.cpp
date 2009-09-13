#include "dcd_loader.h"

#include "dcdplugin.h"

#include "frame_data.h"

#define ERROR fprintf

#define DCDHANDLE(x) ((dcdhandle*)(x))

DCD_Loader::DCD_Loader()
{
    _frame = -1;
    _numframes = -1;
    _natoms = 0;

    _dcd = NULL;
    // _dcd_frame_data.coords = NULL;
}//constructor

DCD_Loader::~DCD_Loader()
{
    if (_dcd != NULL)
        close_file_read(DCDHANDLE(_dcd));
    // if (_dcd_frame_data.coords != NULL)
        // delete _dcd_frame_data.coords;
}//destructor


int DCD_Loader::frame()
{
    return _frame;
}//frame

bool DCD_Loader::frame(int value)
{
    if (_frame == value) return false;
    if (value < 0) value = 0;
    if (value >= _numframes) value = _numframes - 1;
    _frame = value;
    dcdhandle* dcd = DCDHANDLE(_dcd);
    dcd->setsread = value;
    if (value == 0)
        fio_fseek(dcd->fd, header_size, FIO_SEEK_SET);
    else
        fio_fseek(dcd->fd, header_size + firstframesize + framesize * (_frame - 1), FIO_SEEK_SET);
    return true;
}//frame

int DCD_Loader::totalFrames()
{
    return _numframes;
}//totalFrames


bool DCD_Loader::load_dcd_file(const char* filename)
{
    void* v = open_dcd_read(filename, "dcd", &_natoms);
    if (!v)
    {
        ERROR(stderr, "DCD_Loader::load_dcd_file: error reading dcd file '%s'\n",
                filename);
        return false;
    }//if

    _dcd = v;
    _frame = 0;
    dcdhandle* dcd = DCDHANDLE(_dcd);
    _numframes = dcd->nsets;
    // _dcd_frame_data.coords = new float[3 * _natoms];
    header_size = fio_ftell(dcd->fd);
    printf("header: %i\n", header_size);

    extrablocksize = dcd->charmm & DCD_HAS_EXTRA_BLOCK ? 48 + 8 : 0;
    ndims = dcd->charmm & DCD_HAS_4DIMS ? 4 : 3;
    firstframesize = (dcd->natoms+2) * ndims * sizeof(float) + extrablocksize;
    framesize = (dcd->natoms-dcd->nfixed+2) * ndims * sizeof(float) + extrablocksize;
    printf("framesize: %lli\n", framesize);
    return true;
}//load_dcd_file

bool DCD_Loader::load_dcd_frame(Frame_Data* data)
{
    float unitcell[6];
    unitcell[0] = unitcell[2] = unitcell[5] = 1.0f;
    unitcell[1] = unitcell[3] = unitcell[4] = 90.0f;
    dcdhandle* dcd = DCDHANDLE(_dcd);
    if (dcd->setsread == dcd->nsets) return false;
    dcd->setsread += 1;
    _frame += 1;
    int rc = read_dcdstep(dcd->fd, dcd->natoms, dcd->x, dcd->y, dcd->z,
            unitcell, dcd->nfixed, dcd->first, dcd->freeind,
            dcd->fixedcoords, dcd->reverse, dcd->charmm);
    dcd->first = 0;

    if (rc < 0)
    {
        print_dcderror("read_dcdstep", rc);
        return false;
    }

    int id;
    float* bufx = dcd->x;
    float* bufy = dcd->y;
    float* bufz = dcd->z;
    int natoms = data->natoms();
    for (int i = 0; i < natoms; i++)
    {
        id = data->at(i).id;
        data->update(i, bufx[id], bufy[id], bufz[id]);
    }//for
    return true;
}//load_dcd_frame

