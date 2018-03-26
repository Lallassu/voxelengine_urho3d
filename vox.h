#ifndef __VOX_H__
#define __VOX_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

#include "constants.h"

class Vox
{
public:
    Vox();
    ~Vox();
    void LoadVoxFile(const char*);
    int GetSizeX();
    int GetSizeY();
    int GetSizeZ();
    int GetVoxelData(int, int, int);
    int ***voxels;

private:
    unsigned int size_x;
    unsigned int size_y;
    unsigned int size_z;
    int *voxel_data;
    int *colors;
    int num_voxels;

    int ReadInt(std::ifstream*);
    int VoxID(int, int, int, int);
    static const unsigned int default_colors[256];
};

#endif

