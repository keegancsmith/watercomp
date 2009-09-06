#include "frame_data.h"

Frame_Data::Frame_Data()
{
    _natoms = 0;
}//constructor

Frame_Data::~Frame_Data()
{
}//destructor

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

Atom Frame_Data::operator[](int index)
{
    return at(index);
}//operator[]

Atom Frame_Data::at(int index)
{
    // if ((index < 0) || (index >= _natoms))
        // return NULL;
    return data[index];
}//at

