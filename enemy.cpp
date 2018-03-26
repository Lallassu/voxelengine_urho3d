#include "enemy.h"
#include "main.h"

Enemy::Enemy(Context* context_, Vector3 pos): LogicComponent(context_)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
       

    context = context_;
    std::vector<Vox*> vox_chars = GetSubsystem<Game>()->map->vox_chars;
    Vox *vox = vox_chars[(int)Random((float)vox_chars.size())];

    node = GetSubsystem<Game>()->scene_->CreateChild("Enemy");
    node->SetScale(1);

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
    chunk->SetScale(0.7);
    node->SetScale(0.7);
    node->SetEnabled(true);
    node->SetPosition(Vector3(Random(500.0f), 1.0f, Random(500.0f)));
//    node->SetWorldRotation(Quaternion(0.0f, 30.0f, 0.0f));
    node->SetWorldDirection(Vector3(30.0f, 0.0f, 0.0f));
    alive = true;

}

Enemy::~Enemy()
{
}

void Enemy::Hit(Vector3 pos, int power) 
{
    node->SetEnabled(true);
    node->SetPosition(node->GetPosition());
    chunk->Explode(pos, power, constants::PARTICLE_BLOOD);
    alive = false;
}


void Enemy::Update(float time, float timeStep)
{
    if(!alive) {
        return;
    }

    Vector3 p = node->GetPosition();

    // If close to player, follow player
    Vector3 pv = GetSubsystem<Game>()->player->node->GetPosition();
    float distance = utils::Distance(pv, node->GetPosition());
    if(distance < 30) {

        //if(Random(10) > 8) {
        //    GetSubsystem<Game>()->world->Explode(
        //                                         node->GetPosition().x_+Random(5),
        //                                         node->GetPosition().y_+Random(5),
        //                                         node->GetPosition().z_+Random(5),
        //                                         2);
        //}
        node->LookAt(-pv);
        node->Translate(Vector3(0,0, -1)*50*timeStep);
    }

   // if(Random(10) < 5) {
        node->Translate(Vector3(0, 0, -1)*10*timeStep);
   // } else {
   //     node->Translate(Vector3(1, 0, 0)*10*timeStep);
   // }
    p = node->GetPosition();
    if(p.x_ > 500 || p.x_ < 0 || p.z_ < 0 || p.z_ > 500) {
        node->SetPosition(Vector3(10+Random(500), 2, 10+Random(500)));
    }
    p = node->GetPosition();
    int **layout = GetSubsystem<Game>()->map->layout;
    if(layout[(int)p.x_][(int)p.z_] == 1) {
        node->LookAt(Vector3(Random(360), 0, Random(360)));
    }
    node->SetPosition(Vector3(p.x_, p.y_, p.z_));
}
