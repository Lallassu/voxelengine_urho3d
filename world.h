#ifndef __WORLD_H__
#define __WORLD_H__

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Core/Timer.h>
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <math.h>

#include "constants.h"
#include "chunk.h"
#include "vox.h"
#include "phys.h"


using namespace Urho3D;
using namespace std;

class World : public LogicComponent
{
    URHO3D_OBJECT(World, LogicComponent);

    static const int chunk_size_x = 32;
    static const int chunk_size_y = 32;
    static const int chunk_size_z = 32;
    static const int max_chunks_x = 164;
    static const int max_chunks_y = 164;
    static const int max_chunks_z = 164;
    static const int max_chunks_xz = max_chunks_x*max_chunks_z;

public:
    World(Context*);
    ~World();
    void Update(float, float);
    void Explode(int, int, int, int);
    void Destroy(int, int, int);
    bool CheckExists(int, int, int);
    int GetChunkId(int, int, int, bool);
    bool Exists(int, int, int);
    void ClearBlock(int, int, int);
    void ClearBlockBuildBits(int, int, int);
    void AddBlock (int, int, int, int, int, int);
    void AddModelToWorld(Vox*, Vector3, int scale);
    void RemoveBlock (int, int, int, int power);
    void ColorBlock(int x, int y, int z, int r, int g, int b);
    void FadeBlock(int x, int y, int z, float tint);
    void BuildDirty();
    int GetBlocks();
    int GetTriangles();
    int GetChunks();
    int GetDirtyChunks();
    int FFBlock(int, int, int, int, bool);
    PhysPool *block_pool;
    void AddObjectChunk(Chunk*);
    std::vector<Chunk*> object_chunks;
    Vector3 GetBlockColor(int x, int y, int z);
    int GetVariation();
    Color GetPixel(int, int, int);
    void LoadTexture(String);

    void OccludeChunks();


private:
    std::queue<Chunk*> occluder_chunks;
    float occlude_update;
    float phys_update;
    unsigned int cid;
    std::vector<Image*> textures;
    std::vector<int> variation;
    int variation_counter;
    //float Distance(Vector3, Vector3);
    float update_pools;
    unsigned int start_blocks;
    unsigned int current_blocks;
    std::vector<Chunk*> chunks_l;
    Chunk** chunks;
    void FloodFill(Vector3, Vector3, int);
    std::vector<int> ff_list;
    bool hit_floor;
    float update_phys;
};
#endif

