#include "soul.h"
#include "main.h"
#include "utils.h"

Soul::Soul(Context* context_, Vector3 pos): LogicComponent(context_)
{
    context = context_;

    node = GetSubsystem<Game>()->scene_->CreateChild("Soul");

    ResourceCache* cache = context->GetSubsystem<ResourceCache>();
    emitter = node->CreateComponent<ParticleEmitter>();
    emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/soul.xml")->Clone());
    effect = emitter->GetEffect();
    world = GetSubsystem<Game>()->world;

  //  Light* light = node->CreateComponent<Light>();
  //  light->SetLightType(LIGHT_POINT);
  //  light->SetRange(10);
  //  light->SetBrightness(2.5);
  //  light->SetColor(Color(0.5,.2,0.5,1));
  //  light->SetCastShadows(true);
  //  
    node->SetPosition(pos);
    node->SetScale(1);
    alive = true;
}

Soul::~Soul()
{
}

void Soul::PickUp()
{
    alive = false;
    effect->SetActiveTime(0.1);

    ResourceCache* cache = context->GetSubsystem<ResourceCache>();
    ParticleEmitter *p = node->CreateComponent<ParticleEmitter>();
    p->SetEffect(cache->GetResource<ParticleEffect>("Particle/soul.xml")->Clone());
    ParticleEffect *e = p->GetEffect();
    e->SetNumParticles(100);
    e->SetMinEmissionRate(900);
    e->SetMaxEmissionRate(1000);
    e->SetMinVelocity(20);
    e->SetMinTimeToLive(1);
    e->SetMaxTimeToLive(3);
    e->SetActiveTime(0.1);


    GetSubsystem<Game>()->player->AddMana(0.1);
}

void Soul::Update(float time, float timeStep)
{
    if(!alive) { 
        return;
    }

    Vector3 v = Vector3(0,0,0);
    v.x_ += 2-rand()%4;
    v.y_ += 5-rand()%5;
    v.z_ += 2-rand()%4;
    node->RotateAround(v, Quaternion(1, 1, 6-rand()%12), TS_LOCAL);

    // If close to player, follow player
    Vector3 pv = GetSubsystem<Game>()->player->node->GetPosition();
    float distance = utils::Distance(pv, node->GetPosition());
    if(distance < 30) {
        node->LookAt(pv);
        node->Translate(Vector3(0,0, 1)*50*timeStep);
        if(distance < 5) {
            PickUp();
        }
    }

    Vector3 pos = node->GetPosition();
    if(world->Exists(pos.x_, pos.y_, pos.z_)) {
        pos.y_ += MOVE_SPEED*timeStep;
        node->SetPosition(pos);
    }
}
