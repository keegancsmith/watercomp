#pragma once
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "AtomInformation.h"

class PDB_Reader
{
    public:
        static std::vector<AtomInformation> load_pdbfile(std::string filename)
        {
            std::vector<AtomInformation> pdb_information;
            
            FILE* pdb_file = fopen(filename.c_str(), "r");
            
            char buffer[100];
            
            while(!feof(pdb_file))
            {
                for(int i = 0; i < 80; ++i)
                {
                    fscanf(pdb_file, "%c", buffer+i);
                    
                    if(buffer[i] == '\n')
                    {
                        buffer[i] = 0;
                        break;
                    }
                }
                
                /// Anything other than "ATOM  " and "END" are ignored 
                if(strncmp(buffer, "ATOM  ", 6) == 0)
                    pdb_information.push_back(AtomInformation(pdb_information.size(), buffer));
                else if(strncmp(buffer, "END", 3) == 0)
                    break;
            }
            
            fclose(pdb_file);
            
            return pdb_information;
        }
};