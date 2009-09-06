#ifndef PDB_LOADER
#define PDB_LOADER

class Frame_Data;

class PDB_Loader
{
    public:
        PDB_Loader();
        ~PDB_Loader();

        bool load_file(const char* filename, Frame_Data* data);

    private:
        int _water_count;
};//PDB_Loader

#endif
