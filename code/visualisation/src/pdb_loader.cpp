#include "pdb_loader.h"

#include <cstdio>
#include <cstring>

#include "frame_data.h"

#define LINE_LENGTH 256

#define ERROR fprintf


PDB_Loader::PDB_Loader()
{
    _water_count = 0;
}//constructor

PDB_Loader::~PDB_Loader()
{
}//destructor


bool PDB_Loader::load_file(const char* filename, Frame_Data* data)
{
    FILE* f = fopen(filename, "r");
    if (f == NULL)
    {
        ERROR(stderr, "PDB_Loader::load_file error opening '%s'\n", filename);
        return false;
    }//if
    char line[LINE_LENGTH];
    char field[LINE_LENGTH];
    int serial;
    char name[LINE_LENGTH];
    char resname[LINE_LENGTH];
    int resseq;
    double x;
    double y;
    double z;

    Atom a;
    while (!feof(f))
    {
        // fgets(line, LINE_LENGTH, f);
        if (fgets(line, LINE_LENGTH, f) == NULL)
        {
            //reading last line of file causes 'error' o.O
            //ERROR(stderr, "PDB_Loader::load_file fgets error %i\n", ferror(f));
            fclose(f);
            return true;
        }//if

        //TODO: handle super long line (not really in specification)
        //if (strnlen(line, LINE_LENGTH) == LINE_LENGTH)
        //    handle super long line

        sscanf(line, "%s", field);
        if (strcmp("ATOM", field) == 0)
        {
            sscanf(line, "%s %i %s %s %i %lf %lf %lf", field, &serial, name, resname, &resseq, &x, &y, &z);
            if (strcmp("OH2", name) == 0)
            {
                printf("OH2: %f %f %f\n", x, y, z);
                _water_count += 1;
                a.id = serial;
                a.pos[0] = x;
                a.pos[1] = y;
                a.pos[2] = z;
                data->append(a);
            }//if
        }//if
    }//while
    fclose(f);
    return true;
}//load_file

