#include "phys.h"
#include "main.h"

//=============================================================
//
// SimplePhysBlocks using own basic physics
//
//=============================================================
SimplePhysBlock::SimplePhysBlock(Context* context)
{
    this->context = context;
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();

    node = context->GetSubsystem<Game>()->scene_->CreateChild("Box");
    node->SetPosition(Vector3((float)(rand()%2000+2000), (float)(rand()%2000)+2000, (float)(rand()%2000+2000)));
    scale = 0.2+(float)(rand()%11)/10.0;
    node->SetScale(scale);
    model = node->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Materials/vcolors2.xml"));
    model->SetCastShadows(true);

    model->SetEnabled(false);
    free = true;
}

SimplePhysBlock::~SimplePhysBlock()
{

}

bool SimplePhysBlock::IsTimeout() 
{
    return life <= 0? true: false;
}

void SimplePhysBlock::Bounce(Vector3 pos)
{
 //   World *w = context->GetSubsystem<Game>()->world;
 //   bool bounced = false;
 //   //if (w->Exists(pos.x_, pos.y_+1, pos.z_) || w->Exists(pos.x_, pos.y_, pos.z_) || (bounces > 0 && pos.y_ <= 1)) {
 //   if (w->Exists(pos.x_, pos.y_, pos.z_) || w->Exists(pos.x_, pos.y_+1, pos.z_) || (bounces > 0 && pos.y_ <= 1)) {
 //       bounced = true;
 //       vy *= e;
 //   }
 //   if (w->Exists(pos.x_-1, pos.y_, pos.z_)) {
 //       bounced = true;
 //       vx *= -e;
 //   }
 //   if (w->Exists(pos.x_+1, pos.y_, pos.z_)) {
 //       bounced = true;
 //       vx *= e;
 //   }
 //   if (w->Exists(pos.x_, pos.y_, pos.z_-1)) {
 //       bounced = true;
 //       vz *= -e;
 //   }
 //   if (w->Exists(pos.x_, pos.y_, pos.z_+1)) {
 //       bounced = true;
 //       vz *= e;
 //   }
 //   if(bounced) {
 //       bounces--;
 //       e += 0.05;
 //   }
}


void SimplePhysBlock::Update(float time, float delta)
{
    if(free) { 
        return; 
    }

    life -= delta;

    Vector3 p = node->GetPosition();


    if (life > 0 && p.y_ > -50) { 
        World *w = context->GetSubsystem<Game>()->world;
        if(w->Exists(p.x_, p.y_, p.z_)) { // && scale
            vy *= -1;
            vy /= 1.8;
            vx /= 1.5;
            vz /= 1.5;
           // if(fly_time.GetMSec(true) > 2000) {
           //     //w->Explode(p.x_, p.y_, p.z_, scale*5);
           //    // Disable();
           // }
            if(ptype == constants::PARTICLE_BLOOD) {
                Color c = GetBloodColor();
                if(Random(10) > 2) {
                    w->AddBlock((int)round(p.x_), (int)round(p.y_-1), (int)round(p.z_), c.r_, c.g_, c.b_);
                }
                if(Random(10) > 2) {
                    w->AddBlock((int)round(p.x_+1), (int)round(p.y_-1), (int)round(p.z_), c.r_, c.g_, c.b_);
                }
                if(Random(10) > 2) {
                    w->AddBlock((int)round(p.x_), (int)round(p.y_-1), (int)round(p.z_+1), c.r_, c.g_, c.b_);
                }
                if(Random(10) > 2) {
                    w->AddBlock((int)round(p.x_), (int)round(p.y_-1), (int)round(p.z_-1), c.r_, c.g_, c.b_);
                }
                if(Random(10) > 2) {
                    w->AddBlock((int)round(p.x_-1), (int)round(p.y_-1), (int)round(p.z_), c.r_, c.g_, c.b_);
                }
            }
        }
        vx += (fx/mass)*gravity * delta;
        vy += (fy/mass)*gravity * delta;
        vz += (fz/mass)*gravity * delta;
        p.x_ += vx * delta;
        p.y_ += vy * delta;
        p.z_ += vz * delta;

        node->SetPosition(p);

        if(fx >= 0) {
            fx -= 1.5; 
            if(fx < 0) {
                fx = 0;
            }
        }
        if(fx <= 0) {
            fx += 1.5; 
            if(fx > 0) {
                fx = 0;
            }
        }
        if(fz >= 0) { 
            fz -= 1.5; 
            if(fz < 0) {
                fz = 0;
            }
        }
        if(fz <= 0) { 
            fz += 1.5; 
            if(fz > 0) {
                fz = 0;
            }
        }
       
        if((int)vx == 0 && (int)vz == 0) {
            node->SetRotation(Quaternion(0,0,0,1));
            Vector3 pos = node->GetPosition();
            World *w = context->GetSubsystem<Game>()->world;
            if(w->Exists((int)round(pos.x_), (int)round((pos.y_)), (int)round(pos.z_))) {
                if((int)scale == 1) {
                    w->AddBlock((int)round(pos.x_), (int)round(pos.y_)+1, (int)round(pos.z_), color.x_, color.y_, color.z_);
                }
                Disable();
            }
        } else {
            node->Pitch(vx);
            node->Yaw(vy);
            node->Roll(vz);
        }
    } else {
        Disable();
    }
}

void SimplePhysBlock::Disable()
{
   // node->SetPosition(Vector3((float)(rand()%2000+2000), (float)(rand()%2000)+2000, (float)(rand()%2000+2000)));
    //Node* n = node->GetChild("fire");
    //ParticleEmitter *p = (ParticleEmitter*)n->GetComponent<ParticleEmitter>();
    //std::cout << p << std::endl;
    
    //p->SetEmitting(false);
   // node->RemoveAllChildren("Light");
    
   //3 node->RemoveChild(node->RemoveComponent(node->GetComponent<Light>());
    //Node *n = node->GetChild("Light");
    node->RemoveComponent(node->GetComponent<Light>());


    model->SetEnabled(false);
    free = true;
}


bool SimplePhysBlock::IsFree()
{
    return free;
}

Vector4 SimplePhysBlock::GetColor()
{
    Material* m = model->GetMaterial();
    Variant v = m->GetShaderParameter("MatDiffColor");
    return v.GetVector4();
}

Vector3 SimplePhysBlock::GetPosition()
{
    return node->GetPosition();
}

void SimplePhysBlock::Set(Vector3 pos, Vector3 velocity, Vector3 color_, int power, int ptype_)
{
    if(power == 0) {
        power = 1;
    }
    model->SetEnabled(true);
    node->SetPosition(pos);
    color = color_;
    SharedPtr<Material> m = model->GetMaterial()->Clone();
    m->SetShaderParameter("MatDiffColor",Vector4(color.x_/255.0f, color.y_/255.0f, color.z_/255.0f, 0.2f));
    model->SetMaterial(m);

    ptype = ptype_;
    free = false;
    vx = 10-rand()%20;
    vy = power*2+rand()%(power*2);
    vz = 10-rand()%20;
    fx = 0;
    fy = 1+Random(20);
    fz = 0;
    mass = 0.5;
    life = 5+rand()%10;
    gravity = -9.82;
    node->SetRotation(Quaternion(0,0,0,1));

    fly_time = Timer();

    if(ptype == constants::PARTICLE_FIRE && power > 1) {
        if(Random(1000) < 10) {
            ResourceCache* cache = context->GetSubsystem<ResourceCache>();
            ParticleEmitter* emitter=node->CreateComponent<ParticleEmitter>();
            emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/torch_fire.xml"));

            if(Random(10) < 3) {
                // Light
                Light* light = node->CreateComponent<Light>();
                light->SetLightType(LIGHT_POINT);
                light->SetRange(5);
                light->SetBrightness(1.2);
                light->SetColor(Color(0.9,.5,0.0,1));
                light->SetCastShadows(false);
            }
        }
    } else if(ptype == constants::PARTICLE_BLOOD) {
        Color c = GetBloodColor();
        m->SetShaderParameter("MatDiffColor",Vector4(c.r_/255.0f, c.g_/255.0f, c.b_/255.0f, 1.0f));
    }


    fx = 20-Random(40);
    fz = 20-Random(40);
      //  if(Random(10) < 2) {
      //      Node* n_smoke=node->CreateChild("smoke");
      //      n_smoke->Translate(Vector3(0, 0, 0));
      //      n_smoke->Scale(scale);
      //      ParticleEmitter* emitter2=n_particle->CreateComponent<ParticleEmitter>();
      //      emitter2->SetEffect(cache->GetResource<ParticleEffect>("Particle/smoke.xml"));
      //  }
}

//=============================================================
//
// Physblocs using the urho3d physics
//
//=============================================================
PhysBlock::PhysBlock(Context* context) //: LogicComponent(context)
{
    this->context = context;
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();

    node = context->GetSubsystem<Game>()->scene_->CreateChild("Box");
    node->SetPosition(Vector3((float)(rand()%1000+1000), (float)(rand()%1000)+1000, (float)(rand()%1000+1000)));
    float s = 0.1+(float)(rand()%11)/10.0;
    node->SetScale(s);
    model = node->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Materials/vcolors.xml"));
    model->SetCastShadows(true);

    // Create physics components, use a smaller mass also
    shape = node->CreateComponent<CollisionShape>();
    shape->SetBox(Vector3::ONE);
    //shape->SetSphere(s);

    body = node->CreateComponent<RigidBody>();
    body->SetCollisionEventMode(COLLISION_NEVER);
    body->SetMass(10.1f);
    //body->SetLinearRestThreshold(5.0f);
    //body->SetLinearDamping(0.4f);
   // body->SetUseGravity(true);
    body->SetFriction(30.0f);
    body->SetRollingFriction(20.0f);
    body->SetCollisionMask(1);
    body->SetCollisionLayer(2);


    model->SetEnabled(false);
    free = true;
}

PhysBlock::~PhysBlock()
{

}

void PhysBlock::Disable()
{
    //body->ReleaseBody();
   // node->SetPosition(Vector3((float)(rand()%1000+1000), (float)(rand()%1000)+1000, (float)(rand()%1000+1000)));
    model->SetEnabled(false);
    free = true;
}

bool PhysBlock::IsFree()
{
    return free;
}

bool PhysBlock::IsTimeout() 
{
    float end = context->GetSubsystem<Game>()->time->GetElapsedTime();
    if(end >= end_time) {
        //Disable();
        return true;
    }
    return false;
}

bool PhysBlock::IsActive()
{
    return body->IsActive();
}

Vector4 PhysBlock::GetColor()
{
    Material* m = model->GetMaterial();
    Variant v = m->GetShaderParameter("MatDiffColor");
    //Vector4 c = v.GetVector4();
    //return Vector3(c.x_, c.y_, c.z_);
    return v.GetVector4();
}

Vector3 PhysBlock::GetPosition()
{
    return body->GetPosition();
}

void PhysBlock::Set(Vector3 pos, Vector3 velocity, Vector3 color)
{
    end_time = context->GetSubsystem<Game>()->time->GetElapsedTime() + (1+rand() % 5);
    

    node->SetPosition(pos);
    SharedPtr<Material> m = model->GetMaterial()->Clone();
    m->SetShaderParameter("MatDiffColor",Vector4(color.x_/255.0f, color.y_/255.0f, color.z_/255.0f, 0.2f));
    model->SetMaterial(m);
    body->SetLinearVelocity(velocity*(float)OBJECT_VELOCITY);
    //body->SetLinearVelocity(cameraNode_->GetRotation() * Vector3(0.0f, 0.25f, 1.0f) * OBJECT_VELOCITY);
    model->SetEnabled(true);
    free = false;
}

//=============================================================
//
// PhysPool 
//
//=============================================================
PhysPool::PhysPool(Context* context, int maxBlocks, int maxSBlocks): LogicComponent(context)
{
    for(int i = 0; i < maxBlocks; i++) {
        blocks.push_back(new PhysBlock(context));
    }

    for(int i = 0; i < maxSBlocks; i++) {
        simple_blocks.push_back(new SimplePhysBlock(context));
    }
}

PhysPool::~PhysPool()
{
    for(int i = 0; i < blocks.size(); i++) {
        delete blocks[i];
    }

    for(int i = 0; i < simple_blocks.size(); i++) {
        delete simple_blocks[i];
    }
}

SimplePhysBlock* PhysPool::GetSimpleBlock(int idx) 
{
    return simple_blocks[idx];
}


PhysBlock* PhysPool::GetBlock(int idx) 
{
    return blocks[idx];
}

void PhysPool::Update(float time, float delta)
{
    for(int i = 0; i < simple_blocks.size(); i++) {
        simple_blocks[i]->Update(time, delta);
    }
}

int PhysPool::SimpleSize() 
{
    return simple_blocks.size();
}

int PhysPool::Size() 
{
    return blocks.size();
}

void PhysPool::AddSimple(Vector3 pos, Vector3 velo, Vector3 color, int power, int type)
{
    // Round-robin
    if(++rr_sblock >= simple_blocks.size()) {
        rr_sblock = 0;
    }
    if(!simple_blocks[rr_sblock]->free) {
        simple_blocks[rr_sblock]->Disable();
    }
    simple_blocks[rr_sblock]->Set(pos, velo, color, power, type);
}

void PhysPool::Add(Vector3 pos, Vector3 velo, Vector3 color)
{
    // Round-robin
    if(++rr_block >= blocks.size()) {
        rr_block = 0;
    }
    blocks[rr_block]->Set(pos, velo, color);
}

void PhysPool::AddSimpleRelative(Node* n, Vector3 pos, Vector3 velo, Vector3 color, int power)
{
    // Round-robin
    if(++rr_sblock >= simple_blocks.size()) {
        rr_sblock = 0;
    }

    simple_blocks[rr_sblock]->node->SetParent(n);
    simple_blocks[rr_sblock]->Set(pos, velo, color, power, 0);
    simple_blocks[rr_sblock]->node->SetParent(context_->GetSubsystem<Game>()->scene_);
}

void PhysPool::AddRelative(Node* n, Vector3 pos, Vector3 velo, Vector3 color)
{
    // Round-robin
    if(++rr_block >= blocks.size()) {
        rr_block = 0;
    }

    blocks[rr_block]->node->SetParent(n);
    blocks[rr_block]->Set(pos, velo, color);
    blocks[rr_block]->node->SetParent(context_->GetSubsystem<Game>()->scene_);
}
