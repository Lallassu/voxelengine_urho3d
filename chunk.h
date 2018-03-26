#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/WorkQueue.h>

#include <Urho3D/IO/Log.h>

#include "constants.h"
#include "utils.h"

using namespace Urho3D;
using namespace utils;

class Chunk : public LogicComponent
{
    friend void ProcessBuild(const WorkItem* item, unsigned threadIndex);
    URHO3D_OBJECT(Chunk, LogicComponent);
        
public:
    Chunk(Context*);
    ~Chunk();
    void Init(int, int, int, int, int, unsigned int, unsigned int, unsigned int);
    static void RegisterObject(Context* context);
    virtual void Start();
    void Build();
    virtual void Update(float);
    int GetId();
    bool Exists(int, int, int);
    void AddBlock (int, int, int, int, int, int);
    void AddBlock2 (int, int, int, int, int, int);
    void AddBlockColorLocal (int x, int y, int z, int r, int g, int b);
    void AddBlockColor (int x, int y, int z, int r, int g, int b);
    void AddBlockFade (int x, int y, int z, float tint);
    void SetBlock (int, int, int, int);
    void RemoveBlock (int, int, int, int);
    void RemoveBlock2 (int, int, int, Vector3 pos, int type);
    void ClearBlock (int, int, int);
    void ClearBlockBuildBits (int, int, int);
    int GetBlock(int x, int y, int z);
    bool IsDirty();
    int GetTriangles();
    int GetBlocks();
    void AddRigidBody(int, int, int, int);
    Vector3 GetPosition();
    Vector3 GetWorldPosition();
    Quaternion GetRotation();
    int GetSizeX();
    int GetSizeY();
    int GetSizeZ();
    void Hit2(Vector3, int);
    void Hit(Vector3);
    int GetChunkType();
    void FloodFill(int, int, int, Vector3);
    void Explode(Vector3, int, int type);
    float Distance(Vector3, Vector3);
    void SetScale(float scale);
    float GetScale();

    int from_x;
    int from_y;
    int from_z;
    int to_x;
    int to_y;
    int to_z;
   // Node* node;
    SharedPtr<Node> node;
    int cid;

    bool skip_above;
    bool skip_below;
    bool skip_left;
    bool skip_right;
    bool skip_back;
    bool skip_front;

    bool IsInView();
    bool dirty;
    void SetEnabled(bool val);
    void CreateModel();

private:
    int blockSize;
    float scale;
    bool isBuilt;
    float update;
    int collisions;
    int max_collisions;
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
    SharedPtr<Model> model;
    SharedPtr<VertexBuffer> vb;
    SharedPtr<IndexBuffer> ib;
    //VertexBuffer* vb;
    //IndexBuffer* ib;
    SharedPtr<Geometry> geom;
    PODVector<VertexElement> elements;
    SharedPtr<CollisionShape> shape;
    SharedPtr<RigidBody> body;
    SharedPtr<StaticModel> object;

    int no_of_triangles;
    int no_of_blocks;
    int type;
    bool SameColor(int, int);
    unsigned int size_x;
    unsigned int size_y;
    unsigned int size_z;
    int*** blocks;

    bool isBuildReady;
    Vector<Vector3> vertices;
    Vector<Vector3> colors;
   // Vector<unsigned short>indexData; // Share for all chunks from main
    Vector<float>vertexData;
};
#endif

