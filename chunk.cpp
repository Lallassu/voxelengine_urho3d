#include "chunk.h"
#include "main.h"
#include <Urho3D/Core/Timer.h>

//=============================================================
//
//=============================================================
int Chunk::GetChunkType() 
{
    return type;
}

void ProcessBuild(const WorkItem* item, unsigned threadIndex)
{
    Chunk* chunk = reinterpret_cast<Chunk*>(item->aux_);
    //std::cout << "Build: " << chunk->cid << " Thread: " << threadIndex <<std::endl;
   // HiresTimer t = HiresTimer();
    chunk->Build();
    //std::cout << "Build:" << t.GetUSec(false) << " ms"<< std::endl;
}

void Chunk::Update(float time) 
{
    update += time;
    if(update < 0.05) {  // 20 fps
        return;
    }
    update = 0;
    if(dirty && !isBuildReady) {
        // Build in thread.
        WorkQueue* queue = GetSubsystem<WorkQueue>();

        SharedPtr<WorkItem> item = queue->GetFreeItem();
        item->priority_ = M_MAX_UNSIGNED;
        item->workFunction_ = ProcessBuild;
        item->aux_ = this;

       // LightQueryResult& query = lightQueryResults_[i];
       // query.light_ = lights_[i];

       // item->start_ = &query;
        dirty = false;
        queue->AddWorkItem(item);
    } else if(isBuildReady) {
        //HiresTimer t = HiresTimer();
        CreateModel();
        //std::cout << "Create:" << t.GetUSec(false) << " ms"<< std::endl;
    }
}

void Chunk::RegisterObject(Context* context)
{
 //   context->RegisterFactory<Chunk>();
}

void Chunk::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    if(type == constants::CHUNK_TYPE_WORLD) {
        return;
    }

    Vector3 v = node->GetPosition();

    World *w = GetSubsystem<Game>()->world;
    for(int x = v.x_-2; x < v.x_+5; x++) {
        for(int z = v.z_-2; z < v.z_+5; z++) {
            if(Random(50) < 1) {
                Color c = GetBloodColor();
                w->ColorBlock(x+(float)(rand()%10-5), v.y_-1, z+(float)(rand()%10-5), c.r_, c.g_, c.b_);
                if(Random(50) < 1) {
                    w->block_pool->AddSimple(Vector3(x, v.y_, z),
                                             Vector3((float)(rand()%10-5),
                                                     -(float)(rand()%20),
                                                     (float)(rand()%10-5)),
                                             Vector3(c.r_,c.g_,c.b_), 1, constants::PARTICLE_BLOOD);
                }
            }
        }
    }


    collisions++;
    if(no_of_blocks < constants::MIN_DESTROY_BLOCKS && collisions > max_collisions) {
        Hit(Vector3(0,0,0));
    }
  //  } else {
  //      if(rand() % 10 < 1) {
  //          using namespace NodeCollision;
  //          MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());
  //          //while (!contacts.IsEof())
  //         // {
  //              Vector3 pos = contacts.ReadVector3();
  //              World *w = GetSubsystem<Game>()->world;
  //              w->Explode(pos.x_, pos.y_, pos.z_, 3);
  //         // }
  //      }
  //  }
}

////=============================================================
//// Chunk constructor/destructor
////=============================================================
Chunk::Chunk(Context* context): LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Chunk::Start()
{
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Chunk, HandleNodeCollision));
}

void Chunk::Init(int id_, int type_, int x, int y, int z, unsigned int sx, unsigned int sy, unsigned int sz)
{

    collisions = 0;
    max_collisions = 25+rand()% 20;
    dirty = true;
    from_x = x;
    from_y = y;
    from_z = z;
    to_x = x+sx-1;
    to_y = y+sy-1;
    to_z = z+sz-1;
    size_x = sx;
    size_y = sy;
    size_z = sz;
    cid = id_;
    type = type_;
    blockSize = 1; // Hardcoded for now
    isBuildReady = false;

    blocks = new int **[size_x]();
    for (int i = 0; i < size_x; i++) {
        blocks[i] = new int *[size_y]();
        for (int j = 0; j < size_y; j++) {
            blocks[i][j] = new int [size_z]();
            for(int n = 0; n < size_z; n++) {
                blocks[i][j][n] = 0;
            }
        }
    }
    model = new Model(context_);
    vb = new VertexBuffer(context_);
    ib = new IndexBuffer(context_);
    geom = new Geometry(context_);

    isBuilt = false;

    skip_above = false;
    skip_below = false;
    skip_left = false;
    skip_right = false;
    skip_back = false;
    skip_front = false;
}

Chunk::~Chunk()
{
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {
            delete[] blocks[i][j];
        }
        delete[] blocks[i];
    }
    delete[] blocks;

    vb->Release();
    ib->Release();
    body->ReleaseBody();
    shape->ReleaseShape();
    GetSubsystem<Game>()->scene_->GetComponent<PhysicsWorld>()->RemoveCachedGeometry(model);
    object->SetEnabled(false);
    node->Remove();
}

//=============================================================
//
//=============================================================
bool Chunk::IsInView()
{
    if(isBuilt) {
        return object->IsInView();
    }
    return false;
}

float Chunk::GetScale() 
{
    return scale;
}

void Chunk::SetScale(float scale_) 
{
    if(scale != scale_) {
        node->SetScale(scale_);
        scale = scale_;
    }
}

//=============================================================
// Create model
//=============================================================
void Chunk::CreateModel() 
{
   // if(type == constants::CHUNK_TYPE_OBJECT) {
   //     std::cout << "Create model: " << no_of_triangles << std::endl;
   // }

    isBuildReady = false;
    if(vertices.Size() == 0) {
        node->SetEnabled(false);
        return;
    } else {
        node->SetEnabled(true);
    }
    
    if(!isBuilt) {
        vb->SetShadowed(true);
        ib->SetShadowed(true);
        //model->SetBoundingBox(BoundingBox(Vector3(-size_x, -size_y, -size_z), Vector3(size_x, size_y, size_z)));
        int bb = 0;
        if(size_x > size_y && size_x > size_z) {
            bb = size_x;
        } else if(size_y > size_x && size_y > size_z) {
            bb = size_y;
        } else {
            bb = size_z;
        }
        model->SetBoundingBox(BoundingBox(0, (float)bb));
        model->SetNumGeometries(1);

        elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
        elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
        elements.Push(VertexElement(TYPE_VECTOR3, SEM_COLOR));

        geom->SetVertexBuffer(0, vb);
        geom->SetIndexBuffer(ib);

        model->SetGeometry(0, 0, geom);

      //  node = GetSubsystem<Game>()->scene_->CreateChild("FromScratchObject");
        node->SetPosition(Vector3(from_x, from_y, from_z));
        object = node->CreateComponent<StaticModel>();
        object->SetOccluder(true);
        object->SetOccludee(true);
        object->SetShadowDistance(1000);
        object->SetDrawDistance(2000);
        //object->DrawOcclusion(GetSubsystem<Game>()->oc_buffer);

      //  std::cout << "B_OCC: " << object->GetOcclusionLodLevel() << endl;
      //  object->SetOcclusionLodLevel(10);
      //  std::cout << "A_OCC: " << object->GetOcclusionLodLevel() << endl;

        // Only subscribe for small chunks
       // if(type == constants::CHUNK_TYPE_WORLD) {
       //     body->SetCollisionEventMode(COLLISION_ACTIVE);
       // } else if(no_of_blocks <= constants::MIN_DESTROY_BLOCKS) {
       //     body->SetCollisionEventMode(COLLISION_ALWAYS);
       // }

       // if(type == constants::CHUNK_TYPE_OBJECT) {
       //     // Add to object list
       //   //  w->AddObjectChunk(this);
       // }
    } else {
        //vb->Release();
        //ib->Release();
    }
    
    vb->SetSize(vertices.Size(), elements);
    vb->SetData(&vertexData[0]);


    ib->SetSize(vertices.Size(), false);
    ib->SetData(&GetSubsystem<Game>()->indexData[0]);

    geom->SetDrawRange(TRIANGLE_LIST, 0, vertices.Size());


    Vector<SharedPtr<VertexBuffer> > vertexBuffers;
    Vector<SharedPtr<IndexBuffer> > indexBuffers;
    vertexBuffers.Push(vb);
    indexBuffers.Push(ib);

    PODVector<unsigned> morphRangeStarts;
    PODVector<unsigned> morphRangeCounts;
    morphRangeStarts.Push(0);
    morphRangeCounts.Push(0);
    model->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
    model->SetIndexBuffers(indexBuffers);

    object->SetModel(model);
    ResourceCache* cache=GetSubsystem<ResourceCache>();
    object->SetMaterial(cache->GetResource<Material>("Materials/vcolors.xml"));
    object->SetCastShadows(true);

  
  //  GetSubsystem<Game>()->scene_->GetComponent<PhysicsWorld>()->RemoveCachedGeometry(model);
    if(no_of_triangles > 0) {
       // shape->SetTriangleMesh(model);
        isBuilt = true;
    } else {
     //   body->ReleaseBody();
     //   shape->ReleaseShape();
        node->SetEnabled(false);
        isBuilt = false;
    }
   // if(type == constants::CHUNK_TYPE_OBJECT && no_of_triangles > 0) {
   //     shape->SetConvexHull(model);
   // }
    //std::cout << "Model created!\n";
    body = node->CreateComponent<RigidBody>();

    if(type != constants::CHUNK_TYPE_OBJECT) {
        body->SetCollisionLayer(3);
    }

    shape = node->CreateComponent<CollisionShape>();
   // shape->SetTriangleMesh(object->GetModel(), 0);
    shape->SetConvexHull(model);

    if(type == constants::CHUNK_TYPE_OBJECT) {
        SetScale(scale);
    }

}

void Chunk::SetEnabled(bool val) {
    if(isBuilt) {
        node->SetEnabled(val);
    }
}

//=============================================================
//
//=============================================================
void Chunk::Build()
{

  //  std::cout << "Chunk start build" << cid << std::endl;
  //  TBD: Cache world pointer in instance!
    World *w = GetSubsystem<Game>()->world;
    vertices.Clear();
    colors.Clear();


    float r = 0;
    float g = 0;
    float b = 0;

    // Block structure
    // BLOCK: [R-color][G-color][B-color][0][00][back_left_right_above_front]
    //           8bit    8bit     8it   2bit(floodfill)     6bit(faces)
    //

    // Reset faces
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
    //            blocks[x][y][z] &= 0xFFFFFFC0;
                if(blocks[x][y][z] != 0) {            
                    blocks[x][y][z] &= 0xFFFFFF00; 
                }
            }
        }
    }

    no_of_blocks = 0;
    
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                if (blocks[x][y][z] == 0) {
                    continue; // Skip empty blocks
                }
                
                int left = 0, right = 0, above = 0, front = 0, back = 0, below = 0;

                if(!skip_back) {
                    if (z > 0) {
                        if (blocks[x][y][z - 1] != 0) {
                            back = 1;
                            blocks[x][y][z] = blocks[x][y][z] | 0x10;
                        }
                    } else {
                        if (type == constants::CHUNK_TYPE_WORLD) {
                            // Check hit towards other chunks.
                            if (w->Exists((x + from_x) | 0, (y + from_y) | 0, ((z - 1) + from_z) | 0)) {
                                back = 1;
                                blocks[x][y][z] = blocks[x][y][z] | 0x10;
                            }
                        }
                    }
                } else {
                    back = 1;
                }
    
                if(!skip_left) {
                    if (x > 0) {
                        if (blocks[x - 1][y][z] != 0) {
                            left = 1;
                            blocks[x][y][z] = blocks[x][y][z] | 0x8;
                        }
                    } else {
                        if (type == constants::CHUNK_TYPE_WORLD) {
                            // Check hit towards other chunks.
                            if (w->Exists(((x - 1) + from_x) | 0, (y + from_y) | 0, (z + from_z) | 0)) {
                                left = 1;
                                blocks[x][y][z] = blocks[x][y][z] | 0x8;
                            }
                        }
                    }
                } else {
                    left = 1;
                }

                if(!skip_right) {
                    if (x < size_x - 1) {
                        if (blocks[x + 1][y][z] != 0) {
                            right = 1;
                            blocks[x][y][z] = blocks[x][y][z] | 0x4;
                        }
                    } else {
                        if (type == constants::CHUNK_TYPE_WORLD) {
                            if (w->Exists((x + 1 + from_x) | 0, (y + from_y) | 0, (z + from_z) | 0 )){
                                right = 1;
                                blocks[x][y][z] = blocks[x][y][z] | 0x4;
                            }
                        }
                    }
                } else {
                    right = 1;
                }

                if(!skip_below) {
                    // Only check / draw bottom if we are a object!
                    if (type != constants::CHUNK_TYPE_WORLD) {
                        if (y > 0) {
                            if (blocks[x][y - 1][z] != 0) {
                                below = 1;
                                blocks[x][y][z] = blocks[x][y][z] | 0x20; // bit 6
                            }
                        }
                    }  else {
                        if(y >= constants::MAP_FLOOR_HEIGHT) {
                            if (w->Exists((x + from_x) | 0, ((y - 1) + from_y) | 0, (z + from_z) | 0 )) {
                                below = 1;
                                blocks[x][y][z] = blocks[x][y][z] | 0x20;
                            }
                        } else {
                            below = 1;
                        }
                    }
                } else {
                    below = 1;
                }

                if(!skip_above) {
                    if (y < size_y - 1) {
                        if (blocks[x][y + 1][z] != 0) {
                            above = 1;
                            blocks[x][y][z] = blocks[x][y][z] | 0x02;
                        }
                        // if (type == constants::CHUNK_TYPE_WORLD && y+from_y > constants::MAP_WALL_HEIGHT) {
                        //     above = 1;
                        // }
                    } else {
                        if (type == constants::CHUNK_TYPE_WORLD) {
                            // Check hit towards other chunks.
                            if(y+from_y < constants::MAP_WALL_HEIGHT){
                                if (w->Exists((x + from_x) | 0, ((y + 1) + from_y) | 0, (z + from_z) | 0 )) {
                                    above = 1;
                                    blocks[x][y][z] = blocks[x][y][z] | 0x02;
                                }
                            } else {
                                above = 1;
                            }
                        }
                    }
                } else {
                    above = 1;
                }

                if (z < size_z - 1) {
                    if (blocks[x][y][z + 1] != 0) {
                        front = 1;
                        blocks[x][y][z] = blocks[x][y][z] | 0x1;
                    }
                } else {
                    if (type == constants::CHUNK_TYPE_WORLD) {
                        // Check hit towards other chunks.
                        if (w->Exists((x + from_x) | 0, (y + from_y) | 0, ((z + 1) + from_z) | 0 )) {
                            front = 1;
                            blocks[x][y][z] = blocks[x][y][z] | 0x1;
                        }
                    }
                }

              //  if (type == constants::CHUNK_TYPE_WORLD) {
              //      if (front == 1 && left == 1 && right == 1 && above == 1 && back == 1) {
              //          continue; // block is hidden (world)
              //      }
              //  } else {
                    if (front == 1 && left == 1 && right == 1 && above == 1 && back == 1 && below == 1) {
                        continue; // block is hidden (object)
                    }
               // }
               // left = 1;
               // right = 1;
                //above = 1; 
               // front = 1;
               // back = 1;
               // below = 0;

                no_of_blocks++;

                // Draw blocks

                // Only draw below if we are an object
               // if (type != constants::CHUNK_TYPE_WORLD) {
                 if(!below) {
                        // Get below (bit 6)
                        if ((blocks[x][y][z] & 0x20) == 0) {
                            int maxX = 0;
                            int maxZ = 0;
                            int end = 0;

                            for (int x_ = x; x_ < size_x; x_++) {
                                // Check not drawn + same color
                                if ((blocks[x_][y][z] & 0x20) == 0 && SameColor(blocks[x_][y][z], blocks[x][y][z])) {
                                    maxX++;
                                } else {
                                    break;
                                }
                                int tmpZ = 0;
                                for (int z_ = z; z_ < size_z; z_++) {
                                    if ((blocks[x_][y][z_] & 0x20) == 0 && SameColor(blocks[x_][y][z_], blocks[x][y][z])) {
                                        tmpZ++;
                                    } else {
                                        break;
                                    }
                                }
                                if (tmpZ < maxZ || maxZ == 0) {
                                    maxZ = tmpZ;
                                }
                            }
                            for (int x_ = x; x_ < x + maxX; x_++) {
                                for (int z_ = z; z_ < z + maxZ; z_++) {
                                    blocks[x_][y][z_] = blocks[x_][y][z_] | 0x20;
                                }
                            }
                            maxX--;
                            maxZ--;

                            vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize - blockSize, z * blockSize + (blockSize * maxZ)));
                            vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize + (blockSize * maxZ)));
                            vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize - blockSize));

                            vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize - blockSize, z * blockSize + (blockSize * maxZ)));
                            vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize - blockSize));
                            vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize - blockSize, z * blockSize - blockSize));
                         
                            r = ((blocks[x][y][z] >> 24) & 0xFF) / 255.0f;
                            g = ((blocks[x][y][z] >> 16) & 0xFF) / 255.0f;
                            b = ((blocks[x][y][z] >> 8) & 0xFF) / 255.0f;
                            colors.Push(Vector3(r,g,b));
                            colors.Push(Vector3(r,g,b));
                            colors.Push(Vector3(r,g,b));
                            colors.Push(Vector3(r,g,b));
                            colors.Push(Vector3(r,g,b));
                            colors.Push(Vector3(r,g,b));
                        }
                    }
               // }

                if (!above) {
                    // Get above (0010)
                    if ((blocks[x][y][z] & 0x02) == 0) {
                        int maxX = 0;
                        int maxZ = 0;
                        int end = 0;

                        for (int x_ = x; x_ < size_x; x_++) {
                            // Check not drawn + same color
                            if ((blocks[x_][y][z] & 0x02) == 0 && SameColor(blocks[x_][y][z], blocks[x][y][z])) {
                                maxX++;
                            } else {
                                break;
                            }
                            int tmpZ = 0;
                            for (int z_ = z; z_ < size_z; z_++) {
                                if ((blocks[x_][y][z_] & 0x02) == 0 && SameColor(blocks[x_][y][z_], blocks[x][y][z])) {
                                    tmpZ++;
                                } else {
                                    break;
                                }
                            }
                            if (tmpZ < maxZ || maxZ == 0) {
                                maxZ = tmpZ;
                            }
                        }
                        for (int x_ = x; x_ < x + maxX; x_++) {
                            for (int z_ = z; z_ < z + maxZ; z_++) {
                                blocks[x_][y][z_] = blocks[x_][y][z_] | 0x02;
                            }
                        }
                        maxX--;
                        maxZ--;

                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize, z * blockSize + (blockSize * maxZ)));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize, z * blockSize + (blockSize * maxZ)));

                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize, z * blockSize + (blockSize * maxZ)));
                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize, z * blockSize - blockSize));

                        r = ((blocks[x][y][z] >> 24) & 0xFF) / 255.0f;
                        g = ((blocks[x][y][z] >> 16) & 0xFF) / 255.0f;
                        b = ((blocks[x][y][z] >> 8) & 0xFF) / 255.0f;
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                    }
                }
                if (!back) {
                    // back  10000
                    // shadow_blocks.Push([x, y, z]);
                    if ((blocks[x][y][z] & 0x10) == 0) {
                        int maxX = 0;
                        int maxY = 0;

                        for (int x_ = x; x_ < size_x; x_++) {
                            // Check not drawn + same color
                            if ((blocks[x_][y][z] & 0x10) == 0 && SameColor(blocks[x_][y][z], blocks[x][y][z])) {
                                maxX++;
                            } else {
                                break;
                            }
                            int tmpY = 0;
                            for (int y_ = y; y_ < size_y; y_++) {
                                if ((blocks[x_][y_][z] & 0x10) == 0 && SameColor(blocks[x_][y_][z], blocks[x][y][z])) {
                                    tmpY++;
                                } else {
                                    break;
                                }
                            }
                            if (tmpY < maxY || maxY == 0) {
                                maxY = tmpY;
                            }
                        }
                        for (int x_ = x; x_ < x + maxX; x_++) {
                            for (int y_ = y; y_ < y + maxY; y_++) {
                                blocks[x_][y_][z] = blocks[x_][y_][z] | 0x10;
                            }
                        }
                        maxX--;
                        maxY--;
                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize + (blockSize * maxY), z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize - blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize - blockSize));

                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize + (blockSize * maxY), z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize + (blockSize * maxY), z * blockSize - blockSize));

                        r = ((blocks[x][y][z] >> 24) & 0xFF) / 255.0f;
                        g = ((blocks[x][y][z] >> 16) & 0xFF) / 255.0f;
                        b = ((blocks[x][y][z] >> 8) & 0xFF) / 255.0f;
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                    }
                }
                if (!front) {
                    // front 0001
                    if ((blocks[x][y][z] & 0x1) == 0) {
                        int maxX = 0;
                        int maxY = 0;

                        for (int x_ = x; x_ < size_x; x_++) {
                            // Check not drawn + same color
                            if ((blocks[x_][y][z] & 0x1) == 0 && SameColor(blocks[x_][y][z], blocks[x][y][z])) {
                                maxX++;
                            } else {
                                break;
                            }
                            int tmpY = 0;
                            for (int y_ = y; y_ < size_y; y_++) {
                                if ((blocks[x_][y_][z] & 0x1) == 0 && SameColor(blocks[x_][y_][z], blocks[x][y][z])) {
                                    tmpY++;
                                } else {
                                    break;
                                }
                            }
                            if (tmpY < maxY || maxY == 0) {
                                maxY = tmpY;
                            }
                        }
                        for (int x_ = x; x_ < x + maxX; x_++) {
                            for (int y_ = y; y_ < y + maxY; y_++) {
                                blocks[x_][y_][z] = blocks[x_][y_][z] | 0x1;
                            }
                        }
                        maxX--;
                        maxY--;

                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize + (blockSize * maxY), z * blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize + (blockSize * maxY), z * blockSize));
                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize - blockSize, z * blockSize));

                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize + (blockSize * maxY), z * blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize));
                        vertices.Push(Vector3(x * blockSize + (blockSize * maxX), y * blockSize - blockSize, z * blockSize));

                        r = ((blocks[x][y][z] >> 24) & 0xFF) / 255.0f;
                        g = ((blocks[x][y][z] >> 16) & 0xFF) / 255.0f;
                        b = ((blocks[x][y][z] >> 8) & 0xFF) / 255.0f;
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                    }
                }
                if (!left) {
                    if ((blocks[x][y][z] & 0x8) == 0) {
                        int maxZ = 0;
                        int maxY = 0;

                        for (int z_ = z; z_ < size_z; z_++) {
                            // Check not drawn + same color
                            if ((blocks[x][y][z_] & 0x8) == 0 && SameColor(blocks[x][y][z_], blocks[x][y][z])) {
                                maxZ++;
                            } else {
                                break;
                            }
                            int tmpY = 0;
                            for (int y_ = y; y_ < size_y; y_++) {
                                if ((blocks[x][y_][z_] & 0x8) == 0 && SameColor(blocks[x][y_][z_], blocks[x][y][z])) {
                                    tmpY++;
                                } else {
                                    break;
                                }
                            }
                            if (tmpY < maxY || maxY == 0) {
                                maxY = tmpY;
                            }
                        }
                        for (int z_ = z; z_ < z + maxZ; z_++) {
                            for (int y_ = y; y_ < y + maxY; y_++) {
                                blocks[x][y_][z_] = blocks[x][y_][z_] | 0x8;
                            }
                        }
                        maxZ--;
                        maxY--;

                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize + (blockSize * maxZ)));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize + (blockSize * maxY), z * blockSize + (blockSize * maxZ)));

                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize - blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize + (blockSize * maxY), z * blockSize + (blockSize * maxZ)));
                        vertices.Push(Vector3(x * blockSize - blockSize, y * blockSize + (blockSize * maxY), z * blockSize - blockSize));

                        r = ((blocks[x][y][z] >> 24) & 0xFF) / 255.0f;
                        g = ((blocks[x][y][z] >> 16) & 0xFF) / 255.0f;
                        b = ((blocks[x][y][z] >> 8) & 0xFF) / 255.0f;
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                    }
                }
                if (!right) {
                    if ((blocks[x][y][z] & 0x4) == 0) {
                        int maxZ = 0;
                        int maxY = 0;

                        for (int z_ = z; z_ < size_z; z_++) {
                            // Check not drawn + same color
                            if ((blocks[x][y][z_] & 0x4) == 0 && SameColor(blocks[x][y][z_], blocks[x][y][z])) {
                                maxZ++;
                            } else {
                                break;
                            }
                            int tmpY = 0;
                            for (int y_ = y; y_ < size_y; y_++) {
                                if ((blocks[x][y_][z_] & 0x4) == 0 && SameColor(blocks[x][y_][z_], blocks[x][y][z])) {
                                    tmpY++;
                                } else {
                                    break;
                                }
                            }
                            if (tmpY < maxY || maxY == 0) {
                                maxY = tmpY;
                            }
                        }
                        for (int z_ = z; z_ < z + maxZ; z_++) {
                            for (int y_ = y; y_ < y + maxY; y_++) {
                                blocks[x][y_][z_] = blocks[x][y_][z_] | 0x4;
                            }
                        }
                        maxZ--;
                        maxY--;

                        vertices.Push(Vector3(x * blockSize, y * blockSize - blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize, y * blockSize + (blockSize * maxY), z * blockSize + (blockSize * maxZ)));
                        vertices.Push(Vector3(x * blockSize, y * blockSize - blockSize, z * blockSize + (blockSize * maxZ)));

                        vertices.Push(Vector3(x * blockSize, y * blockSize + (blockSize * maxY), z * blockSize + (blockSize * maxZ)));
                        vertices.Push(Vector3(x * blockSize, y * blockSize - blockSize, z * blockSize - blockSize));
                        vertices.Push(Vector3(x * blockSize, y * blockSize + (blockSize * maxY), z * blockSize - blockSize));

                        r = ((blocks[x][y][z] >> 24) & 0xFF) / 255.0f;
                        g = ((blocks[x][y][z] >> 16) & 0xFF) / 255.0f;
                        b = ((blocks[x][y][z] >> 8) & 0xFF) / 255.0f;
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                        colors.Push(Vector3(r,g,b));
                    }
                }
            }
        }
    }

    no_of_triangles = vertices.Size() / 3;
    
  //  indexData.Clear();
    vertexData.Clear();

  //  unsigned short indexData[vertices.Size()];
//    for(int i = 0; i < vertices.Size(); i++) {
//        indexData.Push(i);
//    }
//
    //float vertexData[vertices.Size()*3*3*3]; // add normals + colors
    int c = 0;
    for(int i = 0; i < vertices.Size(); i++) {
        vertexData.Push(vertices[i].x_);
        vertexData.Push(vertices[i].y_);
        vertexData.Push(vertices[i].z_);
        vertexData.Push(0.0f); // normals
        vertexData.Push(0.0f);
        vertexData.Push(0.0f);
        vertexData.Push(colors[i].x_);
        vertexData.Push(colors[i].y_);
        vertexData.Push(colors[i].z_);
    }

    for (unsigned i = 0; i < vertices.Size(); i += 3)
    {
        Vector3& v1 = *(reinterpret_cast<Vector3*>(&vertexData[9 * i]));
        Vector3& v2 = *(reinterpret_cast<Vector3*>(&vertexData[9 * (i + 1)]));
        Vector3& v3 = *(reinterpret_cast<Vector3*>(&vertexData[9 * (i + 2)]));
        Vector3& n1 = *(reinterpret_cast<Vector3*>(&vertexData[9 * i + 3]));
        Vector3& n2 = *(reinterpret_cast<Vector3*>(&vertexData[9 * (i + 1) + 3]));
        Vector3& n3 = *(reinterpret_cast<Vector3*>(&vertexData[9 * (i + 2) + 3]));

        Vector3 edge1 = v1 - v2;
        Vector3 edge2 = v1 - v3;
        n1 = n2 = n3 = edge1.CrossProduct(edge2).Normalized();
    }

    isBuildReady = true;
    dirty = false;
    //std::cout << "Chunk END build" << cid << std::endl;
}

//=============================================================
//
// Getters, setters and helpers
//
//=============================================================
bool Chunk::Exists(int x_, int y_, int z_) 
{
    int x = x_ - from_x * blockSize;
    int y = y_ - from_y * blockSize;
    int z = z_ - from_z * blockSize;

    if(x >= 0 && y >= 0 && z >= 0 && x < size_x && z < size_z && y < size_y) {
        if(((blocks[x][y][z] >> 8) & 0xFFFFFF) != 0) {
            return true;
        }
    }
    return false;
}

//=============================================================
//
//=============================================================
bool Chunk::SameColor(int block1, int block2) 
{
    if (((block1 >> 8) & 0xFFFFFF) == ((block2 >> 8) & 0xFFFFFF) && block1 != 0 && block2 != 0) {
        return true;
    }
    return false;
}

//=============================================================
//
//=============================================================
int Chunk::GetId()
{
    return cid;
}

//=============================================================
//
//=============================================================
void Chunk::AddBlock (int x, int y, int z, int r, int g, int b) 
{
    //if(x < 0 || y < 0 || z < 0) {
    //    return;
    //}
    x -= from_x * blockSize;
    y -= from_y * blockSize;
    z -= from_z * blockSize;
    if(x < 0 || y < 0 || z < 0) { // || z > size_x*blockSize || y > size_y*blockSize || z > size_z*blockSize) {
        return;
    }
    blocks[x][y][z] = (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (0 & 0xFF);
    dirty = true;
}

void Chunk::AddBlock2 (int x, int y, int z, int r, int g, int b) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    blocks[x][y][z] = (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (0 & 0xFF);
    dirty = true;
}

void Chunk::AddBlockColor (int x, int y, int z, int r, int g, int b) 
{
    x -= from_x * blockSize;
    y -= from_y * blockSize;
    z -= from_z * blockSize;
    if(x < 0 || y < 0 || z < 0) { // || x > to_x || y > to_y || z > to_z) {
        return;
    }
    if(((blocks[x][y][z] >> 8) & 0xFFFFFF) != 0) {
        blocks[x][y][z] = (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (0 & 0xFF);
        dirty = true;
    }
}
void Chunk::AddBlockColorLocal (int x, int y, int z, int r, int g, int b) 
{
    if(x < 0 || y < 0 || z < 0 || x > to_x || y > to_y || z > to_z) {
        return;
    }
    if(((blocks[x][y][z] >> 8) & 0xFFFFFF) != 0) {
        blocks[x][y][z] = (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (0 & 0xFF);
        dirty = true;
    }
}

void Chunk::AddBlockFade (int x, int y, int z, float tint) 
{
    x -= from_x * blockSize;
    y -= from_y * blockSize;
    z -= from_z * blockSize;
    if(x < 0 || y < 0 || z < 0 || x > to_x || y > to_y || z > to_z) {
        return;
    }
    if(((blocks[x][y][z] >> 8) & 0xFFFFFF) != 0) {
        int val = blocks[x][y][z];
        int r = (1-tint) * ((val >> 8) & 0xFF);
        int g = (1-tint) * ((val >> 16) & 0xFF);
        int b = (1-tint) * ((val >> 24) & 0xFF);
        blocks[x][y][z] = (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (0 & 0xFF);
        dirty = true;
    }
}

void Chunk::SetBlock (int x, int y, int z, int val) 
{
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    x -= from_x * blockSize;
    y -= from_y * blockSize;
    z -= from_z * blockSize;
    blocks[x][y][z] |= val;
    dirty = true;
}

//=============================================================
//
//=============================================================
void Chunk::RemoveBlock2(int x_, int y_, int z_, Vector3 pos, int type_ = 0) 
{
    if(x_ < 0 || y_ < 0 || z_ < 0) {
        return;
    }
    if(blocks[x_][y_][z_] != 0) {
        int r = (blocks[x_][y_][z_] >> 24) & 0xFF;
        int g = (blocks[x_][y_][z_] >> 16) & 0xFF;
        int b = (blocks[x_][y_][z_] >> 8) & 0xFF;

        World *w = GetSubsystem<Game>()->world;
        Vector3 v =  node->LocalToWorld(Vector3(x_, y_, z_));
        if(Random(5) < 1) {
            w->block_pool->AddSimple(v,
                                     Vector3((float)(rand()%10-5),
                                             -(float)(rand()%20),
                                             (float)(rand()%10-5)),
                                     Vector3(r,g,b), 2, type_);
        }
        //w->block_pool->Add(v, 
        //                   Vector3(0,0,0),
        //                   Vector3(r,g,b));
        //                   //Vector3((float)(rand()%10-5),
        //                   //        -(float)(rand()%20),
        //                   //        (float)(rand()%10-5)),
        blocks[x_][y_][z_] = 0;
        dirty = true;
    }
}

//=============================================================
//
//=============================================================
void Chunk::RemoveBlock(int x, int y, int z, int power) 
{
    int x_ = x - from_x * blockSize;
    int y_ = y - from_y * blockSize;
    int z_ = z - from_z * blockSize;

    if(x_ < 0 || y_ < 0 || z_ < 0) {
        return;
    }

    if(blocks[x_][y_][z_] != 0) {
        int r = (blocks[x_][y_][z_] >> 24) & 0xFF;
        int g = (blocks[x_][y_][z_] >> 16) & 0xFF;
        int b = (blocks[x_][y_][z_] >> 8) & 0xFF;
       
        World *w = GetSubsystem<Game>()->world;
     //   int rnd = w->GetVariation();
      //  if(rnd < 3) {
      //      w->block_pool->Add(Vector3((float)x,(float)y,(float)z), 
      //                         Vector3((float)(rand()%10-5),
      //                                 -(float)(rand()%20),
      //                                 (float)(rand()%10-5)),
      //                         Vector3(r,g,b));
      //  }
        w->block_pool->AddSimple(Vector3((float)x,(float)y,(float)z), 
                                 Vector3((float)(rand()%10-5),
                                         -(float)(rand()%20),
                                         (float)(rand()%10-5)),
                                 Vector3(r,g,b), power, 0);
        blocks[x_][y_][z_] = 0;
        dirty = true;
    }
}

//=============================================================
//
//=============================================================
void Chunk::ClearBlockBuildBits(int x, int y, int z) 
{
    //  int x_ = x - from_x * blockSize;
    //  int y_ = y - from_y * blockSize;
    //  int z_ = z - from_z * blockSize;
    if(x-from_x < 0 || y-from_y < 0 || z-from_z < 0) {
        return;
    }

    blocks[x-from_x][y-from_y][z-from_z] &= 0xFFFFFF00;
    //dirty = true;
}

//=============================================================
//
//=============================================================
void Chunk::ClearBlock(int x, int y, int z) 
{
    x -= from_x * blockSize;
    y -= from_y * blockSize;
    z -= from_z * blockSize;
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    blocks[x][y][z] = 0; //(0 & 0xFF) << 24 | (0 & 0xFF) << 16 | (0 & 0xFF) << 8 | (0 & 0xFF);
    dirty = true;
}

//=============================================================
//
//=============================================================
bool Chunk::IsDirty()
{
    return dirty;
}

//=============================================================
//
//=============================================================
int Chunk::GetBlocks()
{
    return no_of_blocks;
}

//=============================================================
//
//=============================================================
int Chunk::GetBlock(int x, int y, int z)
{
    x -= from_x * blockSize;
    y -= from_y * blockSize;
    z -= from_z * blockSize;
    if(x < 0 || y < 0 || z < 0) {
        return -1;
    }

    return blocks[x][y][z];
}

//=============================================================
//
//=============================================================
Vector3 Chunk::GetPosition()
{
    if(body != NULL) {
        return body->GetPosition();
    }
    return Vector3(0,0,0);
}

//=============================================================
//
//=============================================================
Vector3 Chunk::GetWorldPosition()
{
    if(node != NULL) {
        return node->GetPosition();
    }
    return Vector3(-1,-1,1);
}

//=============================================================
//
//=============================================================
Quaternion Chunk::GetRotation()
{
    return body->GetRotation();
}

//=============================================================
//
//=============================================================
int Chunk::GetTriangles()
{
    return no_of_triangles;
}

//=============================================================
//
//=============================================================
int Chunk::GetSizeX()
{
    return (int)size_x;
}

//=============================================================
//
//=============================================================
int Chunk::GetSizeY()
{
    return (int)size_y;
}

//=============================================================
//
//=============================================================
int Chunk::GetSizeZ()
{
    return (int)size_z;
}

//=============================================================
//
//=============================================================
void Chunk::AddRigidBody(int x, int y, int z, int mass)
{
    if(y < 0) { 
        y = 0;
    }
    node->SetPosition(Vector3((float)x, (float)y, (float)z));
    body->SetMass((float)mass);
    body->SetRestitution(0.5f);
    body->SetAngularDamping(.7f);
    body->SetAngularRestThreshold(0.5f);
    body->SetLinearDamping(1.9f);
    body->SetContactProcessingThreshold(2.0f);
    body->SetLinearVelocity(Vector3(0,-1.82f,0));
    body->SetAngularVelocity(Vector3(0,-9.82f,0));
  //  body->SetMass((float)mass);
  //  body->SetFriction((float)mass);
  //  body->SetRollingFriction(20.0f);
    shape->SetConvexHull(model);
    body->SetUseGravity(true);
}

//=============================================================
//
//=============================================================
//void Chunk::Hit(Vector3 hit_pos)
//{
void Chunk::Explode(Vector3 pos, int power, int type_ = 0)
{
   // Node *new_node = GetSubsystem<Game>()->scene_->CreateChild("TestChunk");
   // new_node->SetParent(node);
   // Vector3 cp = node->GetPosition();
   // cp.x_ -= pos.x_;
   // cp.y_ -= pos.y_;
   // cp.z_ -= pos.z_;
    Vector3 new_pos = node->WorldToLocal(pos);
   // new_node->SetPosition(new_pos);
   // new_node->SetScale(Vector3(5,5,5));
   // ResourceCache* cache=GetSubsystem<ResourceCache>();
   // StaticModel* boxObject2 = new_node->CreateComponent<StaticModel>();
   // boxObject2->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
   // boxObject2->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    if(pos.x_ == 0 && pos.z_ == 0 && pos.y_ == 0) {
        return;
    }

    //World *w = GetSubsystem<Game>()->world;
    //for(int i = 0; i < 5; i++) {
    //    Color c = GetBloodColor();
    //    w->block_pool->AddSimple(Vector3(pos.x_,pos.y_,pos.z_),
    //                             Vector3((float)(rand()%10-5),
    //                                     -(float)(rand()%20),
    //                                     (float)(rand()%10-5)),
    //                             Vector3(c.r_, c.g_, c.b_), 1, constants::PARTICLE_BLOOD);
    //}

    // int x = (int)pos.x_-size_x;
    // int y = (int)pos.y_-size_y;
    // int z = (int)pos.z_-size_z;
    int x = new_pos.x_; 
    int y = new_pos.y_; 
    int z = new_pos.z_; 

    Vector3 own_pos = GetPosition();
    int power2 = power*power;
    std::vector<Vector3> ff;

    int vx = 0, vy = 0, vz = 0, val = 0, offset = 0;
    for(int rx = x-power; rx <= x+power; rx++) {
        vx = pow((rx-x), 2); 
        for(int rz = z-power; rz <= z+power; rz++) {
            vz = pow((rz-z),2)+vx; 
            for(int ry = y-power; ry <= y+power; ry++) {
                if(ry < 0) {
                    continue;
                }
                val = pow((ry-y),2)+vz;

                if(val <= power2) {
                    //if(Exists(rx-own_pos.x_, ry-own_pos.y_, rz-own_pos.z_)) {
                    //   if(Exists(rx, ry, rz)) {
                    if(rx >= 0 && ry >= 0 && rz >= 0 && rx < size_x && rz < size_z && ry < size_y) {
                        if(((blocks[rx][ry][rz] >> 8) & 0xFFFFFF) != 0) {
                            //blocks[rx][ry][rz] = 0;
                            RemoveBlock2(rx, ry, rz, new_pos, type_);
                        }
                    }
                    //    RemoveBlock(rx, ry, rz);
                    //   }
                } else if(val > power2 && val <= power2+4) {
                    if(Random(20) < 1) {
                        Color c = GetBloodColor();
                        AddBlockColorLocal(rx, ry, rz, c.r_, c.g_, c.b_);
                    }
                } else {
                    //if(power > constants::MIN_FF_POWER) {
                        if(rx >= 0 && ry >= 0 && rz >= 0 && rx < size_x && rz < size_z && ry < size_y) {
                            if(((blocks[rx][ry][rz] >> 8) & 0xFFFFFF) != 0) {
                                ff.push_back(Vector3(rx, ry, rz));
                            }
                        }
                    //}
                }
            }
        }
    }

        //dirty = true;
        object->SetEnabled(false);
        node->SetEnabled(false);

    for(int i = 0; i < ff.size(); i++) {
        FloodFill(ff[i].x_, ff[i].y_, ff[i].z_, new_pos);
    }

   // delete this;
    // TBD: Remove complete... Cleanup chunk
}

//=============================================================
//
//=============================================================
void Chunk::FloodFill(int x, int y, int z, Vector3 pos) 
{
    int b = blocks[x][y][z];
    Vector3 p;

    if (b & 0x80) {
        return;
    }

    int max_x = 0;
    int max_y = 0;
    int max_z = 0;

    int min_x = 1000000;
    int min_y = 1000000;
    int min_z = 1000000;

    //int max_distance = 0;
    //int max = rand()%6;

    std::vector<Vector3> stack;
    std::vector<Vector4> result;
    stack.push_back(Vector3(x, y, z));

    //int rrr = rand()%255;
    
    while(stack.size() > 0) {
        p = stack.back();
        stack.pop_back();


        if(p.x_ < 0 || p.y_ < 0 || p.z_ < 0 || p.x_ >= size_x || p.y_ >= size_y || p.z_ >= size_z) {
            continue;
        }
        b = blocks[(int)p.x_][(int)p.y_][(int)p.z_];

        if ((b >> 8) == 0) {
            continue;
        }
        if ((b & 0x80) != 0) {
            continue;
        }
        if ((b & 0x40) != 0) {
            continue;
        }
      //      for(int i = 0; i < result.size(); i++) {
      //          //FFBlock(result[i].x_, result[i].y_, result[i].z_, 0x40, false);
      //      //    ClearBlockBuildBits(result[i].x_, result[i].y_, result[i].z_);
      //      }
      // //     FFBlock(x, y, z, 0x40, false);
      //      return;
      //  }
        if (p.x_ > max_x) { max_x = p.x_; }
        if (p.y_ > max_y) { max_y = p.y_; }
        if (p.z_ > max_z) { max_z = p.z_; }
        if (p.x_ < min_x) { min_x = p.x_; }
        if (p.y_ < min_y) { min_y = p.y_; }
        if (p.z_ < min_z) { min_z = p.z_; }


       // if(max_distance < d) {
       //     max_distance = d;
       // }

        //AddBlock(p.x_, p.y_, p.z_, rrr, 0, 0);
        // Check this
        //int d = Distance(p, Vector3(x,y,z));
        //if(d > 10) {
        //    break;
        //}

        result.push_back(Vector4(p.x_, p.y_, p.z_, b));

    //    if(p.y_ < 2) {
    //        for(int i = 0; i < result.size(); i++) {
    //           // ClearBlockBuildBits(result[i].x_, result[i].y_, result[i].z_);
    //            FFBlock(result[i].x_, result[i].y_, result[i].z_, 0x40, false);
    //        }
    //        FFBlock(x, y, z, 0x40, false);
    //        return;
    //    }

      //  if((int)p.x_ >= 0 && (int)p.y_ >= 0 && (int)p.z_ >= 0 && (int)p.x_ < size_x && (int)p.z_ < size_z && (int)p.y_ < size_y) {
            blocks[(int)p.x_][(int)p.y_][(int)p.z_] |= 0x80;
     //   }

        for(int x_ = -1; x_ <= 1; x_++) {
            for(int y_ = -1; y_ <= 1; y_++) {
                for(int z_ = -1; z_ <= 1; z_++) {
                    if(p.y_+y_ > 0 && p.x_+x_ > 0 && p.z_+z_ > 0) {
                        if(!(p.x_+x_ == p.x_ && p.y_+y_ == p.y_ && p.z_+z_ == p.z_)) {
                            stack.push_back(Vector3(p.x_+x_, p.y_+y_, p.z_+z_));
                        }
                    }
                }
            }
        }
        //stack.push_back(Vector3(p.x_, p.y_+1, p.z_));
        //stack.push_back(Vector3(p.x_, p.y_, p.z_+1));
        //stack.push_back(Vector3(p.x_+1, p.y_, p.z_));
        //stack.push_back(Vector3(p.x_, p.y_, p.z_-1));
        //stack.push_back(Vector3(p.x_-1, p.y_, p.z_));
        //stack.push_back(Vector3(p.x_, p.y_-1, p.z_));
    }

  //  if(max_distance > 10) {
  //      std::cout << "DIST: " << max_distance <<std::endl;
  //      result = small_result;
  //  }

    Vector3 cp = node->GetPosition();
    if(result.size() > 2) {
        Node *new_node = GetSubsystem<Game>()->scene_->CreateChild("WorldChunk");
        Chunk *chunk_new = new_node->CreateComponent<Chunk>();
        chunk_new->node = new_node;
        chunk_new->Init(
                    0,
                    constants::CHUNK_TYPE_OBJECT,
                    0, 0, 0,
                    (max_x-min_x)+1, (max_y-min_y)+1, (max_z-min_z)+1
                   );
        for(int i = 0; i < result.size(); i++) {
            int xx = result[i].x_ - min_x;
            int yy = result[i].y_ - min_y;
            int zz = result[i].z_ - min_z;
            if(Random(10) < 3) {
                Color c = GetBloodColor();
                chunk_new->AddBlock2(xx, yy, zz, c.r_, c.g_, c.b_);
            } else {
                chunk_new->AddBlock2(xx, yy, zz, (int)result[i].w_ >> 24, (int)result[i].w_ >> 16, (int)result[i].w_ >> 8);
            }
           // blocks[xx][yy][zz] = 0;
           // RemoveBlock2(xx,yy,zz, cp);
        }
        Vector3 v = node->LocalToWorld(pos);
        chunk_new->Build();
        if(chunk_new->no_of_triangles == 0) {
            return;
        }
        // TBD: Check what type => Enemy => add to body_parts
        GetSubsystem<Game>()->body_parts.push_back(chunk_new);
        chunk_new->CreateModel();
        chunk_new->SetScale(scale);
        new_node->SetScale(scale);
        chunk_new->node->SetRotation(node->GetRotation());
        //chunk_new->node->SetPosition(node->GetPosition());
        // Set rotation + position from parent.
        //chunk_new->node->SetParent(node);
        //chunk_new->node->SetParent(context_->GetSubsystem<Game>()->scene_);
       // std::cout << max_x/2 << "," << max_y/2 << "," << max_z/2 << std::endl;
        //chunk_new->AddRigidBody(v.x_+max_x/2, v.y_+max_y/2, v.z_+max_z/2, result.size());
        chunk_new->AddRigidBody(cp.x_, cp.y_, cp.z_, result.size());
        chunk_new->body->SetAngularVelocity(body->GetAngularVelocity());
        chunk_new->body->SetLinearVelocity(body->GetLinearVelocity());
        //Build();

    } else {
      //  std::cout << "REMOVE\n";
        for(int i = 0; i < result.size(); i++) {
            int xx = result[i].x_ - min_x;
            int yy = result[i].y_ - min_y;
            int zz = result[i].z_ - min_z;
            RemoveBlock2(xx,yy,zz, cp);
        }
      //  std::cout << "AFER REMOVE\n";
    }
}

float Chunk::Distance(Vector3 from, Vector3 to) 
{
    return sqrt(pow(from.x_-to.x_, 2) + pow(from.y_-to.y_, 2) + pow(from.z_-to.z_, 2));
}

void Chunk::Hit(Vector3 p) 
{
    Vector3 pos = GetPosition();
    World *w = GetSubsystem<Game>()->world;

    for(int x = 0; x < size_x; x++) {
        for(int y = 0; y < size_y; y++) {
            for(int z = 0; z < size_z; z++) {
                if(blocks[x][y][z] >> 8 != 0) {
                    int max = 5;
                    if(no_of_blocks < 200) {
                        max = 8;
                    }
                    if(rand() % 10 < max) {
                        int r = (blocks[x][y][z] >> 24) & 0xFF;
                        int g = (blocks[x][y][z] >> 16) & 0xFF;
                        int b = (blocks[x][y][z] >> 8) & 0xFF;
                        //Vector3 pos_v = Vector3((float)x+pos.x_,(float)y+pos.y_, (float)z+pos.z_);
                        Vector3 pos_v = Vector3((float)x,(float)y, (float)z);
                       // Quaternion q = body->GetRotation();
                       // std::cout << "POS_1: " << pos_v.x_ << ", " << pos_v.y_ << ", " << pos_v.z_ << std::endl;
                       // pos_v = q * pos_v;
                        w->block_pool->AddRelative(node, pos_v, 
                                           Vector3(0,0,0),
                                           Vector3(r,g,b));
                        //w->block_pool->AddRelative(node, pos_v, 
                        //                   Vector3((float)(rand()%10-5),
                        //                           -(float)(rand()%20),
                        //                           (float)(rand()%10-5)),
                        //                   Vector3(r,g,b));
                    }
                    blocks[x][y][z] = 0;
                }
            }
        }
    }
    object->SetEnabled(false);
    dirty = true;
   // dirty = true;
   // Build();
    // TBD: Set orignal rotation?
}

