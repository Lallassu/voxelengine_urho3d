#include "world.h"
#include "main.h"
#include "utils.h"

using namespace utils;

//=============================================================
// World constructor/destructor
//=============================================================
World::World(Context* context): LogicComponent(context)
{
    cid = 0;
    block_pool = new PhysPool(context, 10, 3000);
 //   Chunk::RegisterObject(context);
    chunks = new Chunk*[max_chunks_x*max_chunks_y*max_chunks_z];
    std::cout << "Created Max chunks: " << max_chunks_x*max_chunks_y*max_chunks_z << std::endl;
    for(int i = 0; i < max_chunks_x*max_chunks_y*max_chunks_z; i++) {
        chunks[i] = NULL;
    }

    variation_counter = 0;
    for(int i = 0; i < 1000; i++) {
        variation.push_back(rand()%10);
    }

}

World::~World()
{
   // for (int i = 0; i < chunks.size(); i++) {
   //     chunks[i]->node->RemoveComponent<Chunk>();
   //     delete chunks[i];
   // }
    delete block_pool;
}

int World::GetVariation() 
{
    if(variation_counter < variation.size()) {
        variation_counter++;
    } else {
        variation_counter = 0;
    }
    return variation[variation_counter];
}

//=============================================================
//
//=============================================================
void World::AddModelToWorld(Vox* v, Vector3 pos, int scale)
{
//    LoadTexture("textures/wall1.png");
    LoadTexture("textures/grass.jpg");
    int s = scale;
    for(int x = 0; x < v->GetSizeX(); x++) {
        for(int y = 0; y < v->GetSizeY(); y++) {
            for(int z = 0; z < v->GetSizeZ(); z++) {
                int val = v->GetVoxelData(x,y,z);
                int r = (val >> 8) & 0xFF;
                int g = (val >> 16) & 0xFF;
                int b = (val >> 24) & 0xFF;
                if(r == 0 && g == 0 && b == 0) {
                    continue;
                }
               //     Color c = GetPixel(x, y, 0);
               //     r = c.r_*255;
               //     g = c.g_*255;
               //     b = c.b_*255;
               // }
                for(int x_ = s*x; x_ < s*x+s; x_++) {
                    for(int y_ = s*y; y_ < s*y+s; y_++) {
                        for(int z_ = s*z; z_ < s*z+s; z_++) {
                            int r_,g_,b_;
                            if(r == 0 && g == 255 && b == 0) {
                                Color c = GetPixel(x_, z_, 0);
                                r_ = c.r_*255;
                                g_ = c.g_*255;
                                b_ = c.b_*255;
                            } else {
                                r_ = r;
                                g_ = g;
                                b_ = b;
                            }
                            AddBlock(pos.x_+x_, pos.y_+y_, pos.z_+z_, r_, g_, b_);
                        }
                    }
                }
            }
        }
    }
}

//=============================================================
// Load image
//=============================================================
void World::LoadTexture(String filename)
{
    File file(context_, filename);
    Image *i = new Image(context_);
    i->Load(file);
    textures.push_back(i);
}

Color World::GetPixel(int x, int y, int pos) 
{
    int xx = x - (textures[pos]->GetWidth()* (x/textures[pos]->GetWidth()));
    int yy = y - (textures[pos]->GetHeight()* (y/textures[pos]->GetHeight()));
    return textures[pos]->GetPixel(xx, yy);
}


//=============================================================
//
//=============================================================
void World::AddObjectChunk(Chunk* c)
{
 //   object_chunks.push_back(c);
}


//=============================================================
//  Flat 3D array where:
//    x => (x/chunk_size_x)
//    y => (y/chunk_size_y)
//    z => (y/chunk_size_z)
//    pos_in_flat: chunks[x + y*WIDTH + Z*WIDTH*DEPTH]
//
//=============================================================
int World::GetChunkId(int x, int y, int z, bool create) 
{
    int pos_x = (x/chunk_size_x); 
    int pos_y = (y/chunk_size_y); 
    int pos_z = (z/chunk_size_z); 

    //int pos = pos_x+pos_y*max_chunks_x+pos_z*max_chunks_x*max_chunks_z;
    int pos = pos_x+pos_y*max_chunks_x+pos_z*max_chunks_xz;
 //   std::cout << "CHUNK POS: " << pos << ", " << pos_x << "," << pos_y << "," << pos_z <<std::endl;
   // std::cout << chunks[pos] << std::endl;
    if(chunks[pos] == NULL) {
        if(create) {
     //       std::cout << "Created chunk @ " << pos << std::endl;
            // Create node + chunk etc.
            Node *node = GetSubsystem<Game>()->scene_->CreateChild("WorldChunk");
            Chunk *chunk = node->CreateComponent<Chunk>();
            chunk->node = node;

            chunk->Init(pos,
                        constants::CHUNK_TYPE_WORLD,
                        pos_x * chunk_size_x,
                        pos_y * chunk_size_y,
                        pos_z * chunk_size_z,
                        chunk_size_x, chunk_size_y, chunk_size_z);
            chunks[pos] = chunk;
            chunks_l.push_back(chunk);
        } else {
            return -1;
        }
    }
    return pos;
}

//=============================================================
//
//=============================================================
bool World::Exists(int x, int y, int z) 
{
    if(x < 0 || y < 0 || z < 0) {
        return false;
    }
    int c = GetChunkId(x, y, z, false);
    if(c == -1) {
        return  false;
    }
    if(chunks[c] != NULL) {
        return chunks[c]->Exists(x, y, z);
    } 
    return false;
}

//=============================================================
//
//=============================================================
void World::AddBlock (int x, int y, int z, int r, int g, int b) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    if(r == 0 && g == 0 && b == 0) { 
        return; 
    }
    int c = GetChunkId(x, y, z, true);
    if(c != -1) {
        // Make greyscale
        //if(!((r == 128 && g == 20 && b == 20) ||
        //  (r == 128 && g == 7 && b == 7) ||
        //  (r == 158 && g == 7 && b == 7) ||
        //  (r == 158 && g == 20 && b == 20))) {
        //    r = (int)(r*0.21);
        //    g = (int)(g*0.72);
        //    b = (int)(b*0.07);
        //    r = g = b = (r+g+b);
        //}
        chunks[c]->AddBlock(x, y, z, r, g, b);
    }
}

//=============================================================
//
//=============================================================
void World::ClearBlock(int x, int y, int z) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    int c = GetChunkId(x, y, z, false);
    if(c != -1) {
        chunks[c]->ClearBlock(x, y, z);
    }
}

//=============================================================
//
//=============================================================
void World::ClearBlockBuildBits(int x, int y, int z) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    int c = GetChunkId(x, y, z, false);
    if(c != -1) {
        chunks[c]->ClearBlockBuildBits(x, y, z);
    }
}

//=============================================================
//
//=============================================================
void World::RemoveBlock(int x, int y, int z, int power) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    int c = GetChunkId(x, y, z, false);
    if(c != -1) {
        chunks[c]->RemoveBlock(x, y, z, power);
    }
}

Vector3 World::GetBlockColor(int x, int y, int z) 
{
    if(x < 0 || y < 0 || z < 0) {
        return Vector3(0,0,0); 
    }
    int c = GetChunkId(x, y, z, false);
    int color = 0;
    if(c != -1) {
        color = chunks[c]->GetBlock(x, y, z);
    }
    return Vector3((color >> 24) & 0xFF,
                 (color >> 16) & 0xFF,
                 (color >> 8) & 0xFF);
}

void World::ColorBlock(int x, int y, int z, int r, int g, int b) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    int c = GetChunkId(x, y, z, false);
    if(c != -1) {
        chunks[c]->AddBlockColor(x, y, z, r, g, b);
    }
}

void World::FadeBlock(int x, int y, int z, float tint) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    int c = GetChunkId(x, y, z, false);
    if(c != -1) {
        chunks[c]->AddBlockFade(x, y, z, tint);
    }
}

//=============================================================
//
//=============================================================
void World::Destroy(int x, int y, int z)
{
    int power = 1;
    int power2 = power*power;
    std::vector<Vector3> ff;

    std::vector<Enemy*> ep = GetSubsystem<Game>()->enemies;
    std::vector<Chunk*> bodyp = GetSubsystem<Game>()->body_parts;
    // Check if some enemy are hit
    float dist = 0;
    for(int i = 0; i < ep.size(); i++) {
        dist = Distance(ep[i]->node->GetPosition(), Vector3(x,y,z));
        if(dist <= power) {
            ep[i]->Hit(Vector3(x,y,z), power);
        }
    }
    for(int i = 0; i < bodyp.size(); i++) {
        dist = Distance(bodyp[i]->node->GetPosition(), Vector3(x,y,z));
        if(dist <= power) {
            bodyp[i]->Explode(Vector3(x,y,z), power, 0);
        }
    }

   // Timer t1 = Timer();
    int vx = 0, vz = 0, val = 0;
   // std::cout << "EXPL: " << x << "," << y << "," << z << "," << power << std::endl;
    for(int rx = x-power; rx <= x+power; rx++) {
        vx = pow((rx-x), 2); 
        for(int rz = z-power; rz <= z+power; rz++) {
            vz = pow((rz-z),2)+vx; 
            for(int ry = y-power; ry <= y+power; ry++) {
                if(ry < 0) {
                    continue;
                }
                val = pow((ry-y),2)+vz-GetVariation();

                if(val < power2) {
                    RemoveBlock(rx, ry, rz, 1);
                } else {
                    ff.push_back(Vector3(rx, ry, rz));
                   // }
                  //  if(val <= power2 + 1) {
                  //      int rr = 50; //+ rand()% 50;
                  //      if(Exists(rx, ry, rz)) {
                  //          AddBlock(rx, ry, rz, rr, rr, rr);
                  //      }
                  //  }
                }
            }
        }
    }

// Need to add this to some queue since we need to update chunks before
// performing floodfill. We need to remove the holes.

    std::vector<Vector3> list;
    int id = 0;
    int b = 0;
    for(int i = 0; i < ff.size(); i++) {
        id = GetChunkId(ff[i].x_, ff[i].y_, ff[i].z_, false);
        b = chunks[id]->GetBlock(ff[i].x_, ff[i].y_, ff[i].z_);
        if((b & 0x40) == 0 && (b & 0x80) == 0) {
            FloodFill(ff[i], Vector3(x,y,z), power);
        }
    }
}
void World::Explode(int x, int y, int z, int power)
{
 // Particle effect
    ResourceCache* cache = context_->GetSubsystem<ResourceCache>();
    Node* n_particle = GetSubsystem<Game>()->scene_->CreateChild("smoke");
    n_particle->Translate(Vector3(x, y, z));
    n_particle->Scale(power*2);
    ParticleEmitter* emitter=n_particle->CreateComponent<ParticleEmitter>();
    emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/smoke.xml"));

    int power2 = power*power;
    std::vector<Vector3> ff;

    std::vector<Enemy*> ep = GetSubsystem<Game>()->enemies;
    std::vector<Chunk*> bodyp = GetSubsystem<Game>()->body_parts;
    // Check if some enemy are hit
    float dist = 0;
    for(int i = 0; i < ep.size(); i++) {
        dist = Distance(ep[i]->node->GetPosition(), Vector3(x,y,z));
        if(dist <= power) {
            ep[i]->Hit(Vector3(x,y,z), power);
        }
    }
    for(int i = 0; i < bodyp.size(); i++) {
        dist = Distance(bodyp[i]->node->GetPosition(), Vector3(x,y,z));
        if(dist <= power) {
            bodyp[i]->Explode(Vector3(x,y,z), power, 0);
        }
    }

   // Timer t1 = Timer();
    int vx = 0, vz = 0, val = 0;
   // std::cout << "EXPL: " << x << "," << y << "," << z << "," << power << std::endl;
    for(int rx = x-power; rx <= x+power; rx++) {
        vx = pow((rx-x), 2); 
        for(int rz = z-power; rz <= z+power; rz++) {
            vz = pow((rz-z),2)+vx; 
            for(int ry = y-power; ry <= y+power; ry++) {
                if(ry < 0) {
                    continue;
                }
                val = pow((ry-y),2)+vz-GetVariation();

                if(val < power2) {
                    RemoveBlock(rx, ry, rz, power);
                } else if (val >= power2 && val <= power2 + 5) {
                    FadeBlock(rx, ry, rz, 0.7);
                } else {
                  //  if(power > constants::MIN_FF_POWER) {
                     //   if(Exists(rx, ry, rz)) {
                     //       ff.push_back(Vector3(rx, ry, rz));
                     //   }
                   // }
                  //  if(val <= power2 + 1) {
                  //      int rr = 50; //+ rand()% 50;
                  //      if(Exists(rx, ry, rz)) {
                  //          AddBlock(rx, ry, rz, rr, rr, rr);
                  //      }
                  //  }
                }
            }
        }
    }

//    std::vector<Vector3> list;
//    int id = 0;
//    int b = 0;
//    for(int i = 0; i < ff.size(); i++) {
//        id = GetChunkId(ff[i].x_, ff[i].y_, ff[i].z_, false);
//        b = chunks[id]->GetBlock(ff[i].x_, ff[i].y_, ff[i].z_);
//        if((b & 0x40) == 0 && (b & 0x80) == 0) {
//            FloodFill(ff[i], Vector3(x,y,z), power);
//        }
//    }
}

//=============================================================
//
//=============================================================
void World::FloodFill(Vector3 pos, Vector3 expl_pos, int power) {
    int b = FFBlock(pos.x_, pos.y_, pos.z_, 0, false);
    Vector3 p;

    if (b & 0x80) {
        return;
    }

   // int max = 50+Random(100);

    int max_x = 0;
    int max_y = 0;
    int max_z = 0;

    int min_x = 100000;
    int min_y = 100000;
    int min_z = 100000;

    std::vector<Vector3> stack;
    std::vector<Vector4> result;
    stack.push_back(pos);

    while(stack.size() > 0) {
        p = stack.back();
        stack.pop_back();

      //  if(result.size() > max) {
      //      break;
      //  }

        if(p.x_ < 0 || p.y_ < 0 || p.z_ < 0) {
            continue;
        }
        b = FFBlock(p.x_, p.y_, p.z_, 0, false);

        if(b == -1) {
            continue;
        }
        if ((b >> 8) == 0) {
            continue;
        }
        if ((b & 0x80) != 0) {
            continue;
        }
        if ((b & 0x40) != 0) {
            continue;
        }
        if(p.y_ <= 4) { // TBD: Remove magic number.
            return;
        }

        if (p.x_ > max_x) { max_x = p.x_; }
        if (p.y_ > max_y) { max_y = p.y_; }
        if (p.z_ > max_z) { max_z = p.z_; }
        if (p.x_ < min_x) { min_x = p.x_; }
        if (p.y_ < min_y) { min_y = p.y_; }
        if (p.z_ < min_z) { min_z = p.z_; }

        result.push_back(Vector4(p.x_, p.y_, p.z_, b));
        FFBlock(p.x_, p.y_, p.z_, 0x80, false);
        for(int x_ = -1; x_ <= 1; x_++) {
            for(int y_ = -1; y_ <= 1; y_++) {
                for(int z_ = -1; z_ <= 1; z_++) {
                    if(p.y_+y_ >= 0 && p.x_+x_ >= 0 && p.z_+z_ >= 0) {
                        if(!(p.x_+x_ == p.x_ && p.y_+y_ == p.y_ && p.z_+z_ == p.z_)) {
                            if(utils::Distance(expl_pos, Vector3(p.x_+x_, p.y_+y_, p.z_+z_)) < 50) { // 50? Higher? Lower?
                                stack.push_back(Vector3(p.x_+x_, p.y_+y_, p.z_+z_));
                            }
                        }
                    }
                }
            }
        }
    }

    // NEW CHUNK
    if(result.size() > 5 && result.size() < 1500) {
        Node *node = GetSubsystem<Game>()->scene_->CreateChild("WorldChunk");
        Chunk *chunk = node->CreateComponent<Chunk>();
        chunk->node = node;
        chunk->Init(
                    0,
                    constants::CHUNK_TYPE_OBJECT,
                    0,0,0,
                    (max_x-min_x)+1, (max_y-min_y)+1, (max_z-min_z)+1
                   );
        for(int i = 0; i < result.size(); i++) {
            int xx = result[i].x_ - min_x;
            int yy = result[i].y_ - min_y;
            int zz = result[i].z_ - min_z;
            ClearBlock(result[i].x_, result[i].y_, result[i].z_);
            if(xx <= max_x-min_x && yy <= max_y-min_y && zz <= max_z-min_z) {
                chunk->AddBlock(xx, yy, zz, (int)result[i].w_ >> 24, (int)result[i].w_ >> 16, (int)result[i].w_ >> 8);
            }
        }
        chunk->Build();
        chunk->CreateModel();
        chunk->AddRigidBody(min_x, min_y, min_z, result.size());
    } else if(result.size() <= 5) {
        for(int i = 0; i < result.size(); i++) {
           // int xx = result[i].x_ - min_x;
           // int yy = result[i].y_ - min_y;
           // int zz = result[i].z_ - min_z;
           // RemoveBlock(xx,yy,zz,0);
            RemoveBlock(result[i].x_, result[i].y_, result[i].z_, 0);
        }
    }
}
//=============================================================
//
//=============================================================
int World::FFBlock(int x, int y, int z, int val, bool remove) {
   // for(int i = 0; i < ff_list.size(); i++) {
   //     if (x >= chunks[ff_list[i]]->from_x && x <= chunks[ff_list[i]]->to_x) {
   //         if(z >= chunks[ff_list[i]]->from_z && z <= chunks[ff_list[i]]->to_z) {
   //             if(y >= chunks[ff_list[i]]->from_y && y <= chunks[ff_list[i]]->to_y ) {
   //                 if(remove) {
   //                     chunks[ff_list[i]]->RemoveBlock(x, y, z);
   //                     return -1;
   //                 }
   //                 if(val != 0) {
   //                     chunks[ff_list[i]]->SetBlock(x, y, z, val);
   //                     return -1;
   //                 } else {
   //                     return chunks[ff_list[i]]->GetBlock(x,y,z);
   //                 }
   //             }
   //         }
   //     }
   // }
    // Not in tmp_list, find it.
    int res = GetChunkId(x,y,z, false);

    if(res != -1) {
   //     ff_list.push_back(res);
        if(remove) {
            chunks[res]->RemoveBlock(x,y,z, 0);
        }
        if(val != 0) {
           chunks[res]->SetBlock(x,y,z, val);
        } else {
            return chunks[res]->GetBlock(x,y,z);
        }
    }
    return -1;
}

//=============================================================
//
//=============================================================
//float World::Distance(Vector3 from, Vector3 to) 
//{
//    return sqrt(pow(to.x_-from.x_, 2) + pow(to.y_-from.y_, 2) + pow(to.z_-from.z_, 2));
//}

//=============================================================
//
//=============================================================
void World::BuildDirty() 
{
    for(int i = 0; i < chunks_l.size(); i++) {
        if(chunks_l[i]->IsDirty()) {
            chunks_l[i]->Build();
        }
    }
}

//=============================================================
//
//=============================================================
int World::GetBlocks() 
{
    int bc = 0;
    for(int i = 0; i < chunks_l.size(); i++) {
        bc += chunks_l[i]->GetBlocks();
    }
    return bc;
}

//=============================================================
//
//=============================================================
int World::GetTriangles() 
{
    int tc = 0;
    for(int i = 0; i < chunks_l.size(); i++) {
        tc += chunks_l[i]->GetTriangles();
    }
    return tc;
}

//=============================================================
//
//=============================================================
int World::GetDirtyChunks() 
{
//    int c = 0;
//    for(int i = 0; i < chunks.size(); i++) {
//        c += chunks[i]->IsDirty()? 1 : 0;
//    }
//    return c;
    return 0;
}

//=============================================================
//
//=============================================================
int World::GetChunks() 
{
    return chunks_l.size();
}

//=============================================================
//
//=============================================================
void World::Update(float time, float delta) 
{
        

//    update_phys += delta;
   // if(update_phys > 0.1) {
        block_pool->Update(time, delta);
  //      update_phys = 0;
   // }

   // for(int i = 0; i < block_pool->Size(); i++) {
   //     PhysBlock *b = block_pool->GetBlock(i);
   //     if(!b->IsFree() && !b->IsActive()) {
   //         Vector3 pos = b->GetPosition();
   //         Vector4 col = b->GetColor();
   //         if(Exists(pos.x_, pos.y_-0.5, pos.z_)) {
   //             AddBlock((int)round(pos.x_), (int)round(pos.y_+0.5), (int)round(pos.z_), (int)(col.x_*255), (int)(col.y_*255), (int)(col.z_*255));
   //         }
   //         b->Disable();
   //     } else if(!b->IsFree() && b->IsTimeout()) {
   //         Vector3 pos = b->GetPosition();
   //         Vector4 col = b->GetColor();
   //         if(round(pos.y_) >= 0 && round(pos.z_) >= 0 && round(pos.x_) >= 0) {
   //             // TBD: Check if voxel below exists.
   //             if(Exists(pos.x_, pos.y_-0.5, pos.z_)) {
   //                 AddBlock((int)round(pos.x_), (int)round(pos.y_+0.5), (int)round(pos.z_), (int)(col.x_*255), (int)(col.y_*255), (int)(col.z_*255));
   //             }
   //             b->Disable();
   //         }
   //     } else if(b->GetPosition().y_ < 0) {
   //         b->Disable();
   //     }
   // }
   // update_pools = 0;
// BuildDirty();
}
