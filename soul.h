#ifndef __SOUL_H__
#define __SOUL_H__

#include <string>
#include <vector>
#include <iostream>
#include <math.h>

#include "world.h"
#include "constants.h"
#include "vox.h"
#include "chunk.h"

using namespace Urho3D;

class Soul: public LogicComponent
{
    URHO3D_OBJECT(Soul, LogicComponent);

public:
    Soul(Context*, Vector3 pos);
    ~Soul();
    void Update(float time, float timeStep);
    void PickUp();

private:
    Context *context;
    Node *node;
    ParticleEmitter *emitter;
    ParticleEffect *effect;
    float rot;
    World *world;
    bool alive;
};
#endif

