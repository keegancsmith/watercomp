#include "../../dcd_loader/dcd.h"
#include "DCDWriter.h"

#include <cassert>

DCDWriter::DCDWriter()
{
    atoms = 0;
    dcd = NULL;
}

DCDWriter::~DCDWriter()
{
    if(dcd != NULL)
        close_file_write(dcd);
}

int DCDWriter::natoms() const
{
    return atoms;
}

bool DCDWriter::save_dcd_file(const char* filename, int natoms)
{
    atoms = natoms;
    dcd = (dcdhandle*)open_dcd_write(filename, "dcd", atoms);
}

bool DCDWriter::save_dcd_frame(Frame& data)
{
    /// static int write_timestep(void *v, const molfile_timestep_t *ts);
    
    molfile_timestep_t tmp;
    
    tmp.A = tmp.B = tmp.C = tmp.alpha = tmp.beta = tmp.gamma = 0.0;
    tmp.coords = &data.atom_data[0];
    
    write_timestep(dcd, &tmp);
}
