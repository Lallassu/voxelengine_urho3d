#include "char.h"
#include "main.h"

using namespace constants;

Char::Char(Context* context_): 
    LogicComponent(context_)
{
    scale = 0.7f;
    context = context_;
    input = GetSubsystem<Input>();
    world = GetSubsystem<Game>()->world;
    camera_node = GetSubsystem<Game>()->cameraNode_;
    scene = GetSubsystem<Game>()->scene_;
    free_camera = false;

    camera_node->SetPosition(Vector3(100, 185, 100));

    vox = new Vox();
    vox->LoadVoxFile("models/car1.vox");
    node = GetSubsystem<Game>()->scene_->CreateChild("Car");
    chunk = node->CreateComponent<Chunk>();
    chunk->node = node;
    chunk->Init(0,
                constants::CHUNK_TYPE_OBJECT,
                0,0,0,
                vox->GetSizeX(),
                vox->GetSizeY(),
                vox->GetSizeZ()
               );

    int s = 1;
    for(int x = 0; x < vox->GetSizeX(); x++) {
        for(int y = 0; y < vox->GetSizeY(); y++) {
            for(int z = 0; z < vox->GetSizeZ(); z++) {
                int val = vox->GetVoxelData(x,y,z);
                int r = (val >> 8) & 0xFF;
                int g = (val >> 16) & 0xFF;
                int b = (val >> 24) & 0xFF;

                for(int x_ = s*x; x_ < s*x+s; x_++) {
                    for(int y_ = s*y; y_ < s*y+s; y_++) {
                        for(int z_ = s*z; z_ < s*z+s; z_++) {
                            chunk->AddBlock(x_, y_, z_, r, g, b);
                        }
                    }
                }
            }
        }
    }
    chunk->dirty = true;
    chunk->Build();
    chunk->CreateModel();
    chunk->SetScale(scale);
    node->SetEnabled(true);

   // ResourceCache* cache = context->GetSubsystem<ResourceCache>();
   // emitter = node->CreateComponent<ParticleEmitter>();
   // emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/player.xml"));
   // effect = emitter->GetEffect();
   // Light* light = node->CreateComponent<Light>();
   // light->SetLightType(LIGHT_POINT);
   // light->SetRange(40);
   // light->SetBrightness(1.5);
   // light->SetColor(Color(0.2,.2,0.9,1));
   // light->SetCastShadows(true);
   // 
    node->SetPosition(Vector3(300, 0.3, 300));
    //node->SetScale(2);

   // jump_load = false;
   // jump = false;


    // Default
 //   mana = 2;
 //   souls = 0;
 //   alive = true;
}

Char::~Char()
{

}

void Char::Die()
{
   // mana = 0;
   // ResourceCache* cache = context->GetSubsystem<ResourceCache>();
   // Node *n = scene->CreateChild("explode");
   // n->SetPosition(node->GetPosition());
   // ParticleEmitter *p = n->CreateComponent<ParticleEmitter>();
   // p->SetEffect(cache->GetResource<ParticleEffect>("Particle/die.xml"));
   // ParticleEffect *e = p->GetEffect();
   // e->SetNumParticles(1000);
   // e->SetMinEmissionRate(900);
   // e->SetMaxEmissionRate(1000);
   // e->SetMinVelocity(100);
   // e->SetMinTimeToLive(0.1);
   // e->SetMaxTimeToLive(5);
   // e->SetActiveTime(0.3);

    Vector3 v = node->GetPosition();
    //world->Explode(v.x_, v.y_+10, v.z_, 20);
    world->Explode(v.x_, v.y_, v.z_, 5);
    alive = false;

    // Respawn timer?
}

void Char::Respawn(Vector3 pos) 
{
    
}

void Char::AddMana(float m)
{
   // if(mana + m <= MANA_MAX) {
   //     mana += m;
   //     effect->SetMaxParticleSize(Vector2(mana, mana));
   // } else {
   //     mana = MANA_MAX;
   // }
}

void Char::RemoveMana(float m) 
{
  //  if(mana - m > MANA_MIN) {
  //      mana -= m;
  //  } else {
  //      mana = MANA_MIN;
  //      Die();
  //  }
  //  effect->SetMaxParticleSize(Vector2(mana, mana));
}

void Char::Update(float time, float timeStep)
{
    bool move = false;
    bool falling = false;
    bool explode = false;

    Vector3 pos = node->GetPosition();
    if(world->Exists(pos.x_, pos.y_, pos.z_)) {
        pos.y_ += MOVE_SPEED*timeStep;
        node->SetPosition(pos);
    }

   // if(jump) {
   //     node->Translate(Vector3(0,1, 0)*MOVE_SPEED*timeStep*velo_up);
   //     node->Translate(Vector3(0,0, 1)*MOVE_SPEED*timeStep*velo_forward);
   //     node->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep*velo_back);
   //     node->Translate(Vector3(-1,0,0)*MOVE_SPEED*timeStep*velo_left);
   //     node->Translate(Vector3( 1,0,0)*MOVE_SPEED*timeStep*velo_right);
   // }

    Vector3 nv = node->GetPosition();
    if(!world->Exists(nv.x_, nv.y_-1, nv.z_) && nv.y_ > 1 ) {
        Vector3 pos = node->GetPosition();
        pos.y_ -= MOVE_SPEED*timeStep;
        node->SetPosition(pos);
        velo_up -= 0.1;
        velo_back -= 0.1;
        velo_forward -= 0.1;
        velo_left -= 0.1;
        velo_right -= 0.1;
    } else {
        if(!jump_load) {
            velo_up = 0;
        }
        velo_back = 0;
        velo_forward = 0;
        velo_left = 0;
        velo_right = 0;
        falling = false;
        jump = false;
    }

    if(input->GetKeyDown('W')){
        node->Translate(Vector3(0,0, 1)*MOVE_SPEED*timeStep);
        move = true;
    //    Vector3 pos = node->GetPosition();
    //    world->ColorBlock(pos.x_, pos.y_+1, pos.z_, 40, 40, 30);
    //    world->ColorBlock(pos.x_, pos.y_-1, pos.z_, 40, 40, 30);
        if(!jump) {
            velo_forward = 0.5;
        }
    }
    if(input->GetKeyDown('S')) {
        node->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep);
        move = true;
        if(!jump) {
            velo_back = 0.5;
        }
    }
    if(input->GetKeyDown('A')) {
        node->Translate(Vector3(-1,0,0)*MOVE_SPEED*timeStep);
        move = true;
        if(!jump) {
            velo_left = 0.5;
        }
    }
    if(input->GetKeyDown('D')) {
        node->Translate(Vector3( 1,0,0)*MOVE_SPEED*timeStep);
        move = true;
        if(!jump) {
            velo_right = 0.5;
        }
    }

   // if(input->GetKeyDown(KEY_SPACE) && !jump){
   //     jump_load = true;
   //     velo_up += 0.5;
   // } else {
   //     if(jump_load) {
   //         jump = true;
   //         jump_load = false;
   //         falling = true;
   //         if(velo_up > 3) {
   //             velo_up = 3;
   //         }
   //     }
   // }


    if (input->GetMouseButtonPress(MOUSEB_LEFT)) {
        Vector3 v = node->GetPosition();
        world->Explode(v.x_, v.y_, v.z_, 4+Random(5));
        explode = true;
    
        // Shoot

       // UI* ui = GetSubsystem<UI>();
       // float maxDistance = 600;
       // IntVector2 pos = ui->GetCursorPosition();
       // Graphics* graphics = GetSubsystem<Graphics>();
       // Camera* camera = camera_node->GetComponent<Camera>();
       // Ray cameraRay = camera->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());
       // // Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
       // PODVector<RayQueryResult> results;
       // RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
       // scene->GetComponent<Octree>()->Raycast(query);
       // if (results.Size())
       // {
       //     // Loop to skip all "boxes" so we can shoot through them.
       //     for(int i = 0; i < results.Size(); i++) {
       //         RayQueryResult& result = results[i];
       //         Vector3 res = result.position_;

       //         Node *hit_node = result.drawable_->GetNode();
       //         Chunk *chunk = hit_node->GetComponent<Chunk>();
       //         if(chunk != NULL) {
       //             if(chunk->GetChunkType() == constants::CHUNK_TYPE_OBJECT) {
       //                 chunk->Explode(res, 1);
       //                 break;
       //             } else if(chunk->GetChunkType() == constants::CHUNK_TYPE_WORLD) {
       //                 world->Explode(result.position_.x_, result.position_.y_, result.position_.z_, 5);
       //                 break;
       //             }
       //         }
       //     }
       // }
    }
    if(input->GetKeyDown('M')){
        free_camera = false;
    } 
    if(input->GetKeyDown('N')){
        free_camera = true;
    }

    if(!GetSubsystem<Input>()->IsMouseVisible()) {
        IntVector2 mouseMove = input->GetMouseMove();
        static float yaw_=0;
        yaw_+=MOUSE_SENSITIVITY*mouseMove.x_;
        node->SetRotation(Quaternion(yaw_, Vector3::UP));
    }
    camera_node->Pitch(45.0f);

   // if(move) {
   //     effect->SetMinVelocity(10);
   //     effect->SetMinEmissionRate(300);
   //     effect->SetMaxEmissionRate(300);
   //     effect->SetMinTimeToLive(0.1);
   //     effect->SetMaxTimeToLive(1.2);
   //     effect->SetNumParticles(180);
   // } else {
   //     effect->SetMinVelocity(0.5);
   //     effect->SetMinTimeToLive(0.5);
   //     effect->SetMaxTimeToLive(2);
   //     effect->SetMinEmissionRate(20);
   //     effect->SetMaxEmissionRate(50);
   //     effect->SetNumParticles(180);
   // }
    if(explode) {
    //    ResourceCache* cache = context->GetSubsystem<ResourceCache>();
    //    Node *n = scene->CreateChild("explode");
    //    n->SetPosition(node->GetPosition());
    //    ParticleEmitter *p = n->CreateComponent<ParticleEmitter>();
    //    p->SetEffect(cache->GetResource<ParticleEffect>("Particle/explode.xml"));
    //    ParticleEffect *e = p->GetEffect();
    //    e->SetNumParticles(100);
    //    e->SetMinEmissionRate(900);
    //    e->SetMaxEmissionRate(1000);
    //    e->SetMinVelocity(50);
    //    e->SetMinTimeToLive(1);
    //    e->SetMaxTimeToLive(5);
    //    e->SetActiveTime(0.1);

    //    RemoveMana(0.5);
    }

    // Third person camera: position behind the character
    Vector3 aimPoint = node->GetPosition() + node->GetRotation() * Vector3(0.0f, 1.7f, 0.0f);
    Quaternion dir = node->GetRotation() * Quaternion(45, Vector3::RIGHT);

    // Make up/down smooth
    if(aimPoint.y_ != new_y) {
        if(old_y == 0) {
            old_y = new_y;
        }
        new_y = aimPoint.y_;
    }
    float d = (new_y - old_y)/10;
    old_y += d;
    aimPoint.y_ = old_y;

    // Collide camera ray with static physics objects (layer bitmask 2) to ensure we see the character properly
    Vector3 rayDir = dir * Vector3::BACK;
    float rayDistance = 150;
    PhysicsRaycastResult result;
    scene->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);
    if (result.body_)
        rayDistance = Min(rayDistance, result.distance_);
    rayDistance = Clamp(rayDistance, 45.0f, 150.0f);
    
    // Smooth cam
    if(rayDistance != new_distance) {
        old_distance = new_distance;
        new_distance = rayDistance;
    }

    float diff = (new_distance-old_distance)/20;
    old_pos += diff;
    rayDistance = Clamp(old_pos, 45.0f, 250.0f);

    old_pos = rayDistance;
    camera_node->SetPosition(aimPoint + rayDir * rayDistance);
    camera_node->SetRotation(dir);

    // Check hit on enemies.
    std::vector<Enemy*> enemies = GetSubsystem<Game>()->enemies;
    for(int i = 0; i < enemies.size(); i++) {
        if(utils::Distance(enemies[i]->node->GetPosition(), node->GetPosition()) < 10) {
            enemies[i]->Hit(node->GetPosition(), 10);
        }
        //chunk->Hit(enemies[i]->node->GetPosition());
    }
    // Check hit with world.
    Vector3 p = node->GetWorldPosition();
    int mx = vox->GetSizeX()*scale;
    int mz = vox->GetSizeZ()*scale;
    for(int x = -mx/2; x <= mx/2; x++) {
        for(int z = -mz/2; z <= mz/2; z++) {
            for(int y = 3; y < vox->GetSizeY()*scale+1; y++) {
                if(world->Exists(p.x_+x, p.y_+y, p.z_+z)){
                    world->Destroy(p.x_+x, p.y_+y, p.z_+z);
                }
            }
        }
    }
    

    // Check position if under water
  //  dmg_time += timeStep;

  //  if(node->GetPosition().y_ < 10 && dmg_time > 1 && alive) {
  //      RemoveMana(0.5);
  //      ResourceCache* cache = context_->GetSubsystem<ResourceCache>();
  //      Node* n_particle = GetSubsystem<Game>()->scene_->CreateChild("smoke");
  //      Vector3 pos = node->GetPosition();
  //      n_particle->Translate(Vector3(pos.x_, 10, pos.z_));
  //      n_particle->Scale(10);
  //      ParticleEmitter* emitter=n_particle->CreateComponent<ParticleEmitter>();
  //      emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/smoke.xml"));
  //      dmg_time = 0;
  //  }
}
