#include "../../dcd_loader/dcdplugin.h"
#include "DCDReader.h"

#include <cassert>

#define ERROR fprintf
#define DEBUG fprintf
#define TRACE fprintf
#define DCDHANDLE(x) ((dcdhandle*)(x))

DCDReader::DCDReader()
{
    _dcd = 0;
    _frame = -1;
    _nframes = 0;

    unitcell[0] = unitcell[2] = unitcell[5] = 1.0f;
    unitcell[1] = unitcell[3] = unitcell[4] = 90.0f;
}//constructor

DCDReader::~DCDReader()
{
    if (_dcd!= NULL)
        close_file_read(DCDHANDLE(_dcd));
}//destructor


int DCDReader::frame() const
{
    return _frame;
}//frame

void DCDReader::set_frame(int frame)
{
    //don't do unnecessary seeks and checks
    if (_frame == frame) return;

    if (frame < 0) frame = 0;
    if (frame >= _nframes) frame = _nframes - 1;

    _frame = frame;
    dcdhandle* dcd = DCDHANDLE(_dcd);
    dcd->setsread = _frame;
    if (_frame == 0)
        fio_fseek(dcd->fd, header_size, FIO_SEEK_SET);
    else
        fio_fseek(dcd->fd, header_size + firstframesize + framesize * (_frame - 1), FIO_SEEK_SET);
}//set_frame

size_t DCDReader::nframes() const
{
    return _nframes;
}//nframes

int DCDReader::natoms() const
{
    return _natoms;
}//natoms


bool DCDReader::open_file(const char* filename)
{
    if (_dcd!= NULL)
        close_file_read(DCDHANDLE(_dcd));

    void* v = open_dcd_read(filename, "dcd", &_natoms);
    if (!v)
    {
        ERROR(stderr, "DCDReader::open_file: error reading dcd file '%s'\n",
                filename);
        return false;
    }//if

    _dcd = v;
    _frame = 0;
    dcdhandle* dcd = DCDHANDLE(_dcd);
    _nframes = dcd->nsets;
    // _dcd_frame_data.coords = new float[3 * _natoms];
    header_size = fio_ftell(dcd->fd);
    //TRACE(stdout, "header: %i\n", header_size);

    extrablocksize = dcd->charmm & DCD_HAS_EXTRA_BLOCK ? 48 + 8 : 0;
    ndims = dcd->charmm & DCD_HAS_4DIMS ? 4 : 3;
    firstframesize = (dcd->natoms+2) * ndims * sizeof(float) + extrablocksize;
    framesize = (dcd->natoms-dcd->nfixed+2) * ndims * sizeof(float) + extrablocksize;
    //TRACE(stdout, "framesize: %lli\n", framesize);
    return true;
}//open_file

bool DCDReader::next_frame(Frame& frame)
{
    dcdhandle* dcd = DCDHANDLE(_dcd);
    if (dcd->setsread == dcd->nsets) return false;
    dcd->setsread += 1;
    _frame += 1;
    int arb = fio_ftell(dcd->fd);
    // TRACE(stdout, "ftell: %i\n", arb);
    int rc = read_dcdstep(dcd->fd, dcd->natoms, dcd->x, dcd->y, dcd->z,
            unitcell, dcd->nfixed, dcd->first, dcd->freeind,
            dcd->fixedcoords, dcd->reverse, dcd->charmm);
    dcd->first = 0; //?

    if (rc < 0)
    {
        print_dcderror("read_dcdstep", rc);
        return false;
    }//if

    int i, j;
    float* bufx = dcd->x;
    float* bufy = dcd->y;
    float* bufz = dcd->z;
    int natoms = dcd->natoms;
    
    if(frame.atom_data.size() != natoms*3)
        frame.atom_data.resize(natoms*3);
    
    for (i=0, j=0; i < natoms; i+=1, j+=3)
    {
        frame.atom_data[j  ] = bufx[i];
        frame.atom_data[j+1] = bufy[i];
        frame.atom_data[j+2] = bufz[i];
    }//for
    return true;
}//next_frame

