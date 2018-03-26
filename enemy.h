#ifndef __ENEMY_H__
#define __ENEMY_H__

#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>


#include "world.h"
#include "constants.h"
#include "vox.h"
#include "chunk.h"


using namespace Urho3D;

class Enemy: public LogicComponent
{
    URHO3D_OBJECT(Enemy, LogicComponent);

public:
    Enemy(Context*, Vector3 pos);
    ~Enemy();
    void Update(float time, float timeStep);
    void Hit(Vector3 pos, int power);
    Node *node;
    Node *cnode;

private:
    Context *context;
    Vox *vox;
    Chunk *chunk;
    bool alive;
    SharedPtr<AnimatedModel> model;
    SharedPtr<Animation> animation;
    SharedPtr<AnimationState> animation_state;
    
};
#endif

