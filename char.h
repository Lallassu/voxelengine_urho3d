#ifndef __CHAR_H__
#define __CHAR_H__

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include "constants.h"
#include "world.h"
#include "utils.h"
#include "vox.h"
#include "chunk.h"

using namespace Urho3D;

const static float MOVE_SPEED = 200.0f;
const static float MOUSE_SENSITIVITY = 0.1f;

class Char: public LogicComponent
{
    URHO3D_OBJECT(Char, LogicComponent);

public:
    Char(Context* context);
    ~Char();
    virtual void Update(float time, float timeStep);
    void Die();
    void Respawn(Vector3 pos);
    void AddMana(float m);
    void RemoveMana(float m);
    Node *node;

private:
    Context* context;
    StaticModel *model;
    Input *input;
    World *world;
    Chunk *chunk;
    Node *camera_node;
    Scene *scene;
    ParticleEmitter *emitter;
    ParticleEffect *effect;
    bool free_camera;
    float weapon_var;
    float pitch;
    float velo_up;
    float velo_left;
    float velo_right;
    float velo_back;
    float velo_forward;
    Vox *vox;
    float scale;

    bool jump;
    bool jump_load;

    // For distance to player
    float old_distance;
    float new_distance;
    float old_pos;

    // For camera Y
    float old_y;
    float new_y;

    // Player attr
    float mana;
    int souls;

    float dmg_time;
    bool alive;
};
#endif

