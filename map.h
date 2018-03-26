#ifndef __MAP_H__
#define __MAP_H__

#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <sstream>

#include "world.h"
#include "constants.h"
#include "vox.h"
#include "PerlinNoise.hpp"


using namespace Urho3D;

class Map: public LogicComponent
{
    URHO3D_OBJECT(Map, LogicComponent);

public:
    Map(Context*);
    ~Map();
    void Level1();
    void GenerateBuildings(Vector3 from, Vector3 to);
    void GeneratePark(Vector3 from, Vector3 to);
    void GenerateLight(Vector3 pos, int type);
    void GenerateTrafficLight(Vector3 pos, int type);
    std::vector<Vox*> vox_chars;
    std::vector<Vox*> vox_cars;

    int** layout;

private:
    std::vector<Vector3> house_colors;
    std::vector<Vector3> flower_colors;
    Context *context;
    Vox *voxTree;
    int map_size_x; 
    int map_size_z; 
    int road_size_x;
    int road_size_z;
    int road_pos_y;
    int side_walk_size;
    int lamp_height;
    int street_light_height;
    World *world;
    Scene *scene;
    ResourceCache *cache;
};
#endif

