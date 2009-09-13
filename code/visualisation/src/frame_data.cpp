#include "frame_data.h"

Frame_Data::Frame_Data()
{
    clear();
}//constructor

Frame_Data::~Frame_Data()
{
}//destructor

void Frame_Data::clear()
{
    _natoms = 0;
    for (int i = 0; i < 3; i++)
    {
        bbox[0][i] = 0;
        bbox[1][i] = 0;
        size[i] = 1;
        half_size[i] = 0;
    }//for
    data.resize(0);
}//clear

int Frame_Data::natoms()
{
    return _natoms;
}//natoms

void Frame_Data::natoms(int value)
{
    _natoms = value;
    data.resize(value);
}//natoms


void Frame_Data::append(Atom a)
{
    _natoms += 1;
    data.push_back(a);
}//append

bool Frame_Data::set(int index, Atom a)
{
    if ((index < 0) || (index >= _natoms))
        return false;
    data[index] = a;
    return true;
}//set

Atom& Frame_Data::operator[](int index)
{
    return at(index);
}//operator[]

Atom& Frame_Data::at(int index)
{
    // if ((index < 0) || (index >= _natoms))
        // return NULL;
    return data[index];
}//at

void Frame_Data::update(int index, double x, double y, double z)
{
    data[index].pos[0] = x;
    data[index].pos[1] = y;
    data[index].pos[2] = z;
}//update

void Frame_Data::update_bbox()
{
    if (_natoms <= 0)
        return;

    int i, j;
    for (i = 0; i < 3; i++)
    {
        bbox[0][i] = data[0].pos[i];
        bbox[1][i] = data[0].pos[i];
    }//for

    for (i = 1; i < _natoms; i++)
    {
        for (j = 0; j < 3; j++)
        {
            if (data[i].pos[j] < bbox[0][j])
                bbox[0][j] = data[i].pos[j];
            if (data[i].pos[j] > bbox[1][j])
                bbox[1][j] = data[i].pos[j];
        }//for
    }//for

    max_side = -1;
    for (i = 0; i < 3; i++)
    {
        size[i] = bbox[1][i] - bbox[0][i];
        half_size[i] = size[i] * 0.5;
        if (size[i] > max_side)
            max_side = size[i];
    }//for
}//update_bbox

