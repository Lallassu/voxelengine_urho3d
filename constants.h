#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

namespace constants 
{
    const int CHUNK_TYPE_WORLD = 0;
    const int CHUNK_TYPE_OBJECT = 1;
    const int MIN_FF_POWER = 1;
    const int MIN_DESTROY_BLOCKS = 200;

    const int BLOCK_TYPE_BLOOD = 0;
    const int BLOCK_TYPE_FIRE = 1;
    const int BLOCK_TYPE_REGULAR = 2;
    const int BLOCK_TYPE_SMOKE = 3;

    const int MAP_WALL_HEIGHT = 50;
    const int MAP_FLOOR_HEIGHT = 3;
    const int MAP_SCALE = 5;

    const int PARTICLE_FIRE = 0;
    const int PARTICLE_BLOOD = 1;

    const float MANA_MIN = 0.5;
    const float MANA_MAX = 6.0;

    const float WATER_HEIGHT = 10.0f;
    const float WATER_BOTTOM = 1.0f;
}

#endif

