#ifndef __PHYS_H__
#define __PHYS_H__

#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Core/Timer.h>

#include "chunk.h"
#include "constants.h"
#include "utils.h"

using namespace Urho3D;
using namespace utils;

class SimplePhysBlock
{
public:
    SimplePhysBlock(Context*);
    ~SimplePhysBlock();
    bool IsFree();
    Vector4 GetColor();
    Vector3 GetPosition();
    void Update(float time, float delta);
    void Set(Vector3, Vector3, Vector3, int, int type);
    bool IsActive();
    bool IsTimeout();
    void Disable();
    void Bounce(Vector3 pos);
    Node* node;
    bool free;

private:
    float end_time;
    Timer fly_time;
    Context* context;
    StaticModel* model;
    int type;
    float life;
    float gravity;
    float mass;
    float vy;
    float vx;
    float vz;
    float fx;
    float fy;
    float fz;
    float scale;

    int ptype;

    Vector3 color; 
    Chunk *chunk;

    float damage;
};

// Blocks
class PhysBlock // public LogicComponent
{
    //URHO3D_OBJECT(PhysBlock, LogicComponent);
    static const int OBJECT_VELOCITY = 10;

public:
    PhysBlock(Context*);
    ~PhysBlock();
    void Set(Vector3, Vector3, Vector3);
    Vector3 GetPosition();
    Vector4 GetColor();
    bool IsActive();
    bool IsTimeout();
    void Disable();
    bool IsFree();
    Node* node;
    
private:
    bool free;
    float end_time;
    Context* context;
    StaticModel* model;
    RigidBody* body;
    CollisionShape* shape;
};

// Pool
class PhysPool: public LogicComponent
{
    URHO3D_OBJECT(PhysPool, LogicComponent);

public:
    PhysPool(Context*, int, int);
    ~PhysPool();
    void Update(float time, float delta);
    void Add(Vector3, Vector3, Vector3);
    void AddRelative(Node*, Vector3, Vector3, Vector3);
    void AddSimpleRelative(Node*, Vector3, Vector3, Vector3, int power);
    void AddSimple(Vector3, Vector3, Vector3, int power, int type);
    int Size();
    int SimpleSize();
    PhysBlock* GetBlock(int);
    SimplePhysBlock* GetSimpleBlock(int);

private:
    unsigned int rr_block;
    unsigned int rr_sblock;
    std::vector<SimplePhysBlock*> simple_blocks;
    std::vector<PhysBlock*> blocks;
};
#endif

