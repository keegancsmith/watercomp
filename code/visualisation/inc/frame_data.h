#ifndef FRAME_DATA
#define FRAME_DATA

#include <vector>

struct Atom
{
    int id;
    double pos[3];
};//Atom

class Frame_Data
{
    public:
        double bbox[2][3];
        double size[3];
        double half_size[3];

        Frame_Data();
        ~Frame_Data();

        int natoms();
        void natoms(int value);

        //note, this appends the atom onto the end,
        //meaning that natoms will be incremented
        void append(Atom a);
        bool set(int index, Atom a);
        Atom operator[](int index);
        Atom at(int index);

        void update_bbox();

    private:
        int _natoms;
        std::vector<Atom> data;
};//Frame_Data

#endif
