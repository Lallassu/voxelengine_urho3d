#include "map.h"
#include "main.h"

Map::Map(Context* context_): LogicComponent(context_)
{
    context = context_;
    world = GetSubsystem<Game>()->world;
    scene = GetSubsystem<Game>()->scene_;
    cache = context->GetSubsystem<ResourceCache>();

    map_size_x = 600;
    map_size_z = 600;
    road_size_x = 40;
    road_size_z = 40;
    road_pos_y = 1;
    side_walk_size = 5;
    lamp_height = 35;
    street_light_height = 25;

    house_colors.push_back(Vector3(191, 183, 133));
    house_colors.push_back(Vector3(208, 134, 121));
    house_colors.push_back(Vector3(194, 166, 171));
    house_colors.push_back(Vector3(209, 204, 153));
    house_colors.push_back(Vector3(192, 184, 169));
    house_colors.push_back(Vector3(143, 181, 127));
    house_colors.push_back(Vector3(139, 152, 186));
    house_colors.push_back(Vector3(80, 126, 168));
    house_colors.push_back(Vector3(123, 126, 128));

    flower_colors.push_back(Vector3(146, 43, 134));
    flower_colors.push_back(Vector3(102, 28, 131));
    flower_colors.push_back(Vector3(198, 181, 53));
    flower_colors.push_back(Vector3(192, 31, 0));


    // Map layout.
    layout = new int*[map_size_x];
    for(int x = 0; x < map_size_x; x++) {
        layout[x] = new int[map_size_z];
        for(int z = 0; z < map_size_z; z++) {
            layout[x][z] = 0;
        }
    }

   // for(int i = 1; i <= 37; i++) { 
   //     Vox *v = new Vox(); 
   //     std::ostringstream oss;
   //     oss << "models/car" << i << ".vox";
   //     v->LoadVoxFile(oss.str().c_str());
   //     vox_cars.push_back(v);
   // }

    for(int i = 1; i <= 87; i++) { 
        Vox *v = new Vox(); 
        std::ostringstream oss;
        oss << "models/char" << i << ".vox";
        v->LoadVoxFile(oss.str().c_str());
        vox_chars.push_back(v);
    }
}

Map::~Map()
{

}

void Map::GeneratePark(Vector3 from, Vector3 to)
{
    bool create_fence = (bool)Random(2);
    int y = road_pos_y+1;
    for(int x = from.x_ ; x < to.x_; x++) {
        for(int z = from.z_; z < to.z_; z++) {
            if(x == from.x_ || z == from.z_ || x == to.x_-1 || z == to.z_-1) {
                if(create_fence) {
                    // Fence around park
                    for(int yy = 0; yy < 10; yy++) {
                        if(x % 10 == 0 || z % 10 == 0) {
                            world->AddBlock(x, y+yy, z, 150, 150, 150);
                            world->AddBlock(x, y+10, z, 190, 190, 190);
                        } else if(yy % 4 == 0) {
                            world->AddBlock(x, y+yy, z, 150, 150, 150);
                        }
                    }
                }
            }
            if(x % 20 == 0 && z % 20 == 0 && Random(10) < 1) {
                // Trees
                int base = 5+Random(10)/2;
                int height = 25+Random(20);
                int g = 100+Random(100);
                for(int xx = x-base; xx < x+base; xx++) {
                    for(int zz = z - base; zz < z+base; zz++) {
                        for(int yy = 0; yy < height; yy++) {
                            if(yy < height/3 && xx > x-base/2 && xx < x+base/2 && zz > z-base/2 && zz < z+base/2) { 
                                world->AddBlock(xx, y+yy, zz, 139, 69, 20);
                            } else if(yy >= height/3 && (xx == x-base || xx == x+base-1 || zz == z-base || zz == z+base-1 || yy == height/3 || yy == height-1 )) {
                                world->AddBlock(xx, y+yy, zz, 34, g, 34);
                            }
                        }
                    }
                }
                // Around trees.
                int size = 10+Random(20);
                for(int xx = x - size; xx < x + size; xx++) {
                    for(int zz = z - size; zz < z + size; zz++) {
                        if(xx < from.x_ || xx > to.x_ || zz < from.z_ || zz > to.z_) {
                            continue;
                        }
                        // Grass
                        float dist = utils::Distance(Vector3(xx, y, zz), Vector3(x,y,z));
                        if(Random(size) > dist) {
                            int g = 100+Random(150);
                            if(Random(10) < 1) {
                                world->AddBlock(xx, y+1, zz, 34, g, 34);
                            }
                            world->AddBlock(xx, y, zz, 34, g, 34);
                        }
                        if(Random(50) < 1) {
                            // Flower
                            Vector3 color = flower_colors[(int)Random((float)flower_colors.size())];
                            world->AddBlock(xx+1, y+1, zz, color.x_, color.y_, color.z_);
                            world->AddBlock(xx-1, y+1, zz, color.x_, color.y_, color.z_);
                            world->AddBlock(xx, y+1, zz+1, color.x_, color.y_, color.z_);
                            world->AddBlock(xx, y+1, zz-1, color.x_, color.y_, color.z_);
                            world->AddBlock(xx, y, zz, 255, 255, 255);
                            world->AddBlock(xx, y+1, zz, 255, 255, 255);

                        }
                    }
                }
            }

            // Grass + flowers elsewhere
            if(Random(50) < 1) {  
                int g = 100+Random(150);
                if(Random(10) < 1) {
                    world->AddBlock(x, y+1, z, 34, g, 34);
                }
                world->AddBlock(x, y, z, 34, g, 34);
            }
            if(Random(200) < 1) {
                // Flower
                Vector3 color = flower_colors[(int)Random((float)flower_colors.size())];
                world->AddBlock(x+1, y+1, z, color.x_, color.y_, color.z_);
                world->AddBlock(x-1, y+1, z, color.x_, color.y_, color.z_);
                world->AddBlock(x, y+1, z+1, color.x_, color.y_, color.z_);
                world->AddBlock(x, y+1, z-1, color.x_, color.y_, color.z_);
                world->AddBlock(x, y, z, 200, 200, 200);
                world->AddBlock(x, y+1, z, 200, 200, 200);
            }
        }
    }

    // Place fountain in the park
    // TBD: Generate it!
    int len_x = to.x_ - from.x_;
    int len_z = to.z_ - from.z_;

    int size_x = 0;
    int size_z = 0;
    while(size_x <= len_x/3) {
        size_x = 10+Random(50);
    }
    while(size_z <= len_z/3) {
        size_z = 10+Random(50);
    }

    int from_x = from.x_+len_x/2-size_x/2;
    int to_x = from.x_+len_x/2+size_x/2;
    int from_z = from.z_+len_z/2-size_z/2;
    int to_z = from.z_+len_z/2+size_z/2;

    for(int xx = from_x; xx < to_x; xx++) {
        for(int zz = from_z ; zz < to_z; zz++) {
            for(int yy = y; yy < y+50; yy++) {
                world->ClearBlock(xx, yy, zz);
                if(yy < 6 && ((xx < from_x+4 || xx > to_x-4) || (zz < from_z+4 || zz > to_z-4))) {
                    world->AddBlock(xx, yy, zz, 150, 150, 150);
                } else if(yy == y) {
                    if(zz % 4 <= 1 && xx % 4 <= 1) {
                        world->AddBlock(xx, yy, zz, 125, 150, 250);
                    } else {
                        world->AddBlock(xx, yy, zz, 125, 180, 220);
                    }
                }
            }
        }
    }
    // TBD: Move this to own function(pos, size that also handle removal)
 //   Node *waterNode_ = scene->CreateChild("Water");
 //   waterNode_->SetScale(Vector3(size_x, 1.0f, size_z));
 //   waterNode_->SetPosition(Vector3(from_x+size_x/2, 4, from_z+size_z/2));
 //   StaticModel* water = waterNode_->CreateComponent<StaticModel>();
 //   water->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
 //   water->SetMaterial(cache->GetResource<Material>("Materials/Water.xml"));
 //   water->SetViewMask(0x80000000);

 //   Plane waterPlane_ = Plane(waterNode_->GetWorldRotation() * Vector3(0.0f, 1.5f, 0.0f), waterNode_->GetWorldPosition());
 //   Plane waterClipPlane_ = Plane(waterNode_->GetWorldRotation() * Vector3(0.0f, 1.5f, 0.0f), waterNode_->GetWorldPosition() -
 //                                 Vector3(0.0f, 0.1f, 0.0f));

 //   Node *cameraNode_ = GetSubsystem<Game>()->cameraNode_;
 //   Graphics *graphics_ = GetSubsystem<Game>()->graphics_;

 //   Node *reflectionCameraNode_ = cameraNode_->CreateChild();
 //   Camera* reflectionCamera = reflectionCameraNode_->CreateComponent<Camera>();
 //   reflectionCamera->SetFarClip(800.0);
 //   reflectionCamera->SetViewMask(0x7fffffff); // Hide objects with only bit 31 in the viewmask (the water plane)
 //   reflectionCamera->SetAutoAspectRatio(false);
 //   reflectionCamera->SetUseReflection(true);
 //   reflectionCamera->SetReflectionPlane(waterPlane_);
 //   reflectionCamera->SetUseClipping(true); // Enable clipping of geometry behind water plane
 //   reflectionCamera->SetClipPlane(waterClipPlane_);
 //   reflectionCamera->SetAspectRatio((float)graphics_->GetWidth() / (float)graphics_->GetHeight());

 //   int texSize = 1024;
 //   SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
 //   renderTexture->SetSize(texSize, texSize, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
 //   renderTexture->SetFilterMode(FILTER_BILINEAR);
 //   RenderSurface* surface = renderTexture->GetRenderSurface();
 //   SharedPtr<Viewport> rttViewport(new Viewport(context_, scene, reflectionCamera));
 //   surface->SetViewport(0, rttViewport);
 //   Material* waterMat = cache->GetResource<Material>("Materials/Water.xml");
 //   waterMat->SetTexture(TU_DIFFUSE, renderTexture);

}

void Map::GenerateBuildings(Vector3 from, Vector3 to)
{
    int r = 128;
    int g = 100;
    int b = 100;
    int building_offset = 20;

    // Go through whole part
    int y = road_pos_y+1;
    for(int x = from.x_ ; x < to.x_; x++) {
        for(int z = from.z_; z < to.z_; z++) {
            if((x % building_offset == 0 && z % building_offset == 0 && (x < from.x_+50 || x > to.x_-50)) || 
               (x % building_offset == 0 && z % building_offset == 0 && (x < from.x_+50 || z > to.z_-50)) ||
               (x % building_offset == 0 && z % building_offset == 0 && (z > to.z_-50 || x > to.x_-50)))
            {
                // Check how large section we have to build house on, max_x, max_z
                int max_x = 0;
                int max_z = 0;
                int x_ = x;
                int z_ = z;
                while(!world->Exists(x_++, y, z)) {
                    max_x++;
                }
                while(!world->Exists(x, y, z_++)) {
                    max_z++;
                }
                if(max_x < 25 || max_z < 25) {
                    continue;
                }
                //int depth_x = max_x/2 + Random(max_x/2);
                //int depth_z = max_z/2 + Random(max_z/2);
                int depth_x = 25 + Random(max_x-25);
                int depth_z = 25 + Random(max_z-25);
                if(depth_x > 80) {
                    depth_x = 40+Random(20);
                }
                if(depth_z > 80) {
                    depth_z = 40+Random(20);
                }

                int height = 20+Random(70);

                int r = 0;
                int g = 0;
                int b = 0;
                Vector3 color = house_colors[(int)Random((float)house_colors.size())];
                Vector3 top_color = house_colors[(int)Random((float)house_colors.size())];

                int roof_color = 80+Random(120);
                // Add windows
                int win_size = 10; 

                bool gen_tower = false;
                int gen_middle = (int)Random(2);
                int gen_bottom = (int)Random(2);
                int gen_box = Random(5);
                bool gen_windowsill = (int)Random(2); 
                bool gen_helipad = (int)Random(2);


                for(int xx = x; xx < x+depth_x; xx++) {
                    for(int zz = z; zz < z+depth_z; zz++) {
                        if(!world->Exists(xx, y, zz)) {
                            world->AddBlock(xx, y, zz, top_color.x_, top_color.y_, top_color.z_);
                            for(int yy = y; yy < y+height; yy++) {

                                if(xx == x || xx == x+depth_x-1 || zz == z || zz == z + depth_z -1 || yy == y+height-2) {
                                    if(yy == y+height-1) {
                                        // Top floor border
                                        world->AddBlock(xx, yy, zz, top_color.x_, top_color.y_, top_color.z_);
                                    } else if(yy == y+height-2 && !(xx == x || xx == x+depth_x-1 || zz == z || zz == z + depth_z -1 )) {
                                        // Top floor 
                                        world->AddBlock(xx, yy, zz, roof_color, roof_color, roof_color);
                                        if(gen_box > 0 && Random(100) < 1) {
                                            int xs = 2+Random(8);
                                            int zs = 2+Random(8);
                                            int ys = 2+Random(5);
                                            int ct = 50+Random(150);
                                            for(int xt = 0; xt < xs; xt++) {
                                                for(int yt = 0; yt < 5; yt++) {
                                                    for(int zt = 0; zt < zs; zt++) {
                                                        if(xx+xt > x+1 && xx+xt < x+depth_x-1 && zz+zt > z+1 && zz+zt < z+depth_z-1 && !world->Exists(xx+xt, yy+yt, zz+zt)) {
                                                            world->AddBlock(xx+xt, yy+yt, zz+zt, ct, ct, ct);
                                                        }
                                                    }
                                                }
                                            }
                                            gen_box--;

                                            // Add smoke chimney to roof tops
                                            if(Random(10) < 1) {
                                                ResourceCache* cache = context_->GetSubsystem<ResourceCache>();
                                                Node* n_particle = GetSubsystem<Game>()->scene_->CreateChild("smoke");
                                                n_particle->Translate(Vector3(xx+xs/2, yy+8, zz+zs/2));
                                                for(int y_ = yy+5; y_ < yy+8; y_++) {
                                                    world->AddBlock(xx+xs/2, y_, zz+zs/2, ct, ct, ct);
                                                }
                                                n_particle->Scale(5);
                                                ParticleEmitter* emitter=n_particle->CreateComponent<ParticleEmitter>();
                                                emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/smoke.xml")->Clone());
                                                emitter->GetEffect()->SetActiveTime(0);
                                                emitter->GetEffect()->SetMinTimeToLive(1);
                                                emitter->GetEffect()->SetMaxTimeToLive(5);
                                            }
                                        }

                                        if(height > 70 && gen_helipad && xx < x+depth_x-10 && xx > x+10 && zz > z+10 && zz < z+depth_z-10) {
                                            for(int i = 0; i < 10; i++) {
                                                for(int j = 0; j < 2; j++) {
                                                    for(int k = 0; k < 20; k++) {
                                                        world->ClearBlock(xx+i, yy+k, zz+j);
                                                    }
                                                    world->AddBlock(xx+i, yy+1, zz+j, 250, 250, 250);
                                                    world->AddBlock(xx+i, yy+1, zz+j+5, 250, 250, 250);
                                                    if(i < 5) {
                                                        world->AddBlock(xx+5, yy+1, zz+i, 250, 250, 250);
                                                        world->AddBlock(xx+4, yy+1, zz+i, 250, 250, 250);
                                                    }
                                                }
                                            }
                                            gen_helipad = false;
                                        }
                                        // Red blinking tower
                                        if(!gen_tower && Random(100) < 1 && yy > 60) {
                                            for(int yt = 0; yt < 10; yt++) {
                                                world->AddBlock(xx, yy+yt, zz, 150, 150, 150);
                                            }
                                            world->AddBlock(xx, yy+10, zz, 255, 0, 0);
                                            // TBD: Create blinking light from particle
                                            Node* lightNode = scene->CreateChild();
                                            lightNode->SetPosition(Vector3(xx,yy+10,zz));
                                            Light* light=lightNode->CreateComponent<Light>();
                                            light->SetRange(15);
                                            light->SetLightType(LIGHT_POINT);
                                            light->SetBrightness(2.4);
                                            light->SetColor(Color(0.9,0.0,0.0,1));
                                            light->SetCastShadows(false);
                                            gen_tower = true;
                                        }
                                    } else {
                                        // Handle layout of the building

                                        if(gen_bottom && yy < 5) {
                                            world->AddBlock(xx, yy, zz, top_color.x_, top_color.y_, top_color.z_);
                                        } else {
                                            world->AddBlock(xx, yy, zz, color.x_, color.y_, color.z_);
                                        }


                                        // Windows
                                        // TBD: Variants of windows.
                                        if(xx == x+depth_x-1 && yy > win_size && yy < y+height-win_size && zz > z+win_size && zz < z+depth_z-win_size) {
                                            if(yy % win_size < win_size/2 && zz % win_size < win_size/2) {
                                                world->ClearBlock(xx, yy, zz);
                                                world->AddBlock(xx-1, yy, zz, 182-(height-yy), 203-(height-yy), 218-(height-yy));
                                                if(yy % win_size == 0 && gen_windowsill && height < 50) {
                                                    world->AddBlock(xx+1, yy-1, zz, color.x_, color.y_, color.z_);
                                                    // plant at the window
                                                    if(Random(20) < 1) {
                                                        int g = 100+Random(150);
                                                        world->AddBlock(xx+1, yy, zz, 213, 117, 0);
                                                        world->AddBlock(xx+1, yy+1, zz, 40, g, 40);
                                                    }
                                                }
                                            }
                                        }
                                        if(xx == x && yy > 5 && yy < y+height-5 && zz > z+win_size && zz < z+depth_z-win_size) {
                                            if(yy % win_size < win_size/2 && zz % win_size < win_size/2) {
                                                world->ClearBlock(xx, yy, zz);
                                                world->AddBlock(xx+1, yy, zz, 182-(height-yy), 203-(height-yy), 218-(height-yy));
                                                if(yy % win_size == 0 && gen_windowsill && height < 50) {
                                                    world->AddBlock(xx-1, yy-1, zz, color.x_, color.y_, color.z_);
                                                    if(Random(30) < 1) {
                                                        int g = 100+Random(150);
                                                        world->AddBlock(xx-1, yy, zz, 213, 117, 0);
                                                        world->AddBlock(xx-1, yy+1, zz, 40, g, 40);
                                                    }
                                                }
                                            }
                                        }

                                        if(zz == z && yy > 5 && yy < y+height-5 && xx > x+win_size && xx < x+depth_x-win_size) {
                                            if(yy % win_size < win_size/2 && xx % win_size < win_size/2) {
                                                world->ClearBlock(xx, yy, zz);
                                                world->AddBlock(xx, yy, zz+1, 182-(height-yy), 203-(height-yy), 218-(height-yy));
                                                if(yy % win_size == 0 && gen_windowsill && height < 50) {
                                                    world->AddBlock(xx, yy-1, zz-1, color.x_, color.y_, color.z_);
                                                    if(Random(30) < 1) {
                                                        int g = 100+Random(150);
                                                        world->AddBlock(xx, yy+1, zz-1, 40, g, 40);
                                                        world->AddBlock(xx, yy, zz-1, 213, 117, 0);
                                                    }
                                                }
                                            }
                                        }

                                        if(zz == z+depth_z-1 && yy > 5 && yy < y+height-5 && xx > x+win_size && xx < x+depth_x-win_size) {
                                            if(yy % win_size < win_size/2 && xx % win_size < win_size/2) {
                                                world->ClearBlock(xx, yy, zz);
                                                world->AddBlock(xx, yy, zz-1, 182-(height-yy), 203-(height-yy), 218-(height-yy));
                                                if(yy % win_size == 0 && gen_windowsill && height < 50) {
                                                    world->AddBlock(xx, yy-1, zz+1, color.x_, color.y_, color.z_);
                                                    if(Random(30) < 1) {
                                                        int g = 100+Random(150);
                                                        world->AddBlock(xx, yy+1, zz+1, 40, g, 40);
                                                        world->AddBlock(xx, yy, zz+1, 213, 117, 0);
                                                    }
                                                }
                                            }
                                        }
                                        // Add doors/stairs
                                        // Flowers in front of door?
                                        // Flags?
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
    }
}

void Map::GenerateTrafficLight(Vector3 pos, int type) 
{
    int x_ = pos.x_;
    int z_ = pos.z_;

    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            for(int n = 1; n < street_light_height/3; n++) {
                if(n < 3) {
                    world->AddBlock(x_+i,road_pos_y+n,z_+j, 215, 216, 219);
                }
            }
            if(i == 0 && j == 0) {
                for(int n = 2; n < street_light_height; n++) {
                    world->AddBlock(x_+i,road_pos_y+n,z_+j, 205, 206, 209);
                    if(n == street_light_height-1) {
                        if(type == 0) {
                            for(int q = -2; q < 1; q++) {
                               for(int p = -1; p < 2; p++) {
                                   for(int m = 0; m < 7; m++) {
                                       world->AddBlock(x_-p,road_pos_y+n-m,z_+q, 100, 110, 120);
                                   }
                               }
                           }
                           int rl = 0;
                           int gl = 0;
                           int bl = 0;
                           for(int yl = 1; yl < 6; yl++) {
                               if(yl < 2) {
                                   rl = 250;
                                   gl = 0;
                                   bl = 0;
                               } else if(yl < 4) {
                                   rl = 250;
                                   gl = 250;
                                   bl = 0;
                               } else {
                                   rl = 0;
                                   gl = 250;
                                   bl = 0;
                               }

                               //world->ClearBlock(x_+2,road_pos_y+n-yl,z_+1);
                               world->ClearBlock(x_,road_pos_y+n-yl,z_-2);

                               // world->AddBlock(x_+1,road_pos_y+n-yl,z_+1, rl, gl, bl);
                               world->AddBlock(x_,road_pos_y+n-yl,z_-2, rl, gl, bl);

                               if(yl % 2 == 0) {
                                   //   world->AddBlock(x_+2,road_pos_y+n-yl,z_+1, 100, 110, 120);
                                   world->AddBlock(x_,road_pos_y+n-yl,z_-2, 100, 110, 120);
                               }
                           }
                        } else if(type == 1) {
                            for(int q = -1; q < 2; q++) {
                               for(int p = 0; p < 3; p++) {
                                   for(int m = 0; m < 7; m++) {
                                       world->AddBlock(x_-p,road_pos_y+n-m,z_+q, 100, 110, 120);
                                   }
                               }
                           }
                           int rl = 0;
                           int gl = 0;
                           int bl = 0;
                           for(int yl = 1; yl < 6; yl++) {
                               if(yl < 2) {
                                   rl = 250;
                                   gl = 0;
                                   bl = 0;
                               } else if(yl < 4) {
                                   rl = 250;
                                   gl = 250;
                                   bl = 0;
                               } else {
                                   rl = 0;
                                   gl = 250;
                                   bl = 0;
                               }

                               //world->ClearBlock(x_+2,road_pos_y+n-yl,z_+1);
                               world->ClearBlock(x_-2,road_pos_y+n-yl,z_);

                               // world->AddBlock(x_+1,road_pos_y+n-yl,z_+1, rl, gl, bl);
                               world->AddBlock(x_-2,road_pos_y+n-yl,z_, rl, gl, bl);

                               if(yl % 2 == 0) {
                                   //   world->AddBlock(x_+2,road_pos_y+n-yl,z_+1, 100, 110, 120);
                                   world->AddBlock(x_-2,road_pos_y+n-yl,z_, 100, 110, 120);
                               }
                           }
                       } else if(type == 2) {
                           for(int q = 0; q < 3; q++) {
                               for(int p = -1; p < 2; p++) {
                                   for(int m = 0; m < 7; m++) {
                                       world->AddBlock(x_-p,road_pos_y+n-m,z_+q, 100, 110, 120);
                                   }
                               }
                           }
                           int rl = 0;
                           int gl = 0;
                           int bl = 0;
                           for(int yl = 1; yl < 6; yl++) {
                               if(yl < 2) {
                                   rl = 250;
                                   gl = 0;
                                   bl = 0;
                               } else if(yl < 4) {
                                   rl = 250;
                                   gl = 250;
                                   bl = 0;
                               } else {
                                   rl = 0;
                                   gl = 250;
                                   bl = 0;
                               }

                               //world->ClearBlock(x_+2,road_pos_y+n-yl,z_+1);
                               world->ClearBlock(x_,road_pos_y+n-yl,z_+2);

                               // world->AddBlock(x_+1,road_pos_y+n-yl,z_+1, rl, gl, bl);
                               world->AddBlock(x_,road_pos_y+n-yl,z_+2, rl, gl, bl);

                               if(yl % 2 == 0) {
                                   //   world->AddBlock(x_+2,road_pos_y+n-yl,z_+1, 100, 110, 120);
                                   world->AddBlock(x_,road_pos_y+n-yl,z_+2, 100, 110, 120);
                               }
                           }
                        } else if(type == 3) {
                            for(int q = -1; q < 2; q++) {
                                for(int p = 0; p < 3; p++) {
                                    for(int m = 0; m < 7; m++) {
                                        world->AddBlock(x_+p,road_pos_y+n-m,z_+q, 100, 110, 120);
                                    }
                                }
                            }

                            int rl = 0;
                            int gl = 0;
                            int bl = 0;
                            for(int yl = 1; yl < 6; yl++) {
                                if(yl < 2) {
                                    rl = 250;
                                    gl = 0;
                                    bl = 0;
                                } else if(yl < 4) {
                                    rl = 250;
                                    gl = 250;
                                    bl = 0;
                                } else {
                                    rl = 0;
                                    gl = 250;
                                    bl = 0;
                                }

                                //world->ClearBlock(x_+2,road_pos_y+n-yl,z_+1);
                                world->ClearBlock(x_+2,road_pos_y+n-yl,z_);

                               // world->AddBlock(x_+1,road_pos_y+n-yl,z_+1, rl, gl, bl);
                                world->AddBlock(x_+2,road_pos_y+n-yl,z_, rl, gl, bl);
                                
                                if(yl % 2 == 0) {
                                 //   world->AddBlock(x_+2,road_pos_y+n-yl,z_+1, 100, 110, 120);
                                    world->AddBlock(x_+2,road_pos_y+n-yl,z_, 100, 110, 120);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Map::GenerateLight(Vector3 pos, int type) 
{
    int x_ = pos.x_;
    int z_ = pos.z_;

    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            for(int n = 1; n < lamp_height/3; n++) {
                world->AddBlock(x_+i,road_pos_y+n,z_+j, 205, 206, 209);
            }
            if(i == 0 && j == 0) {
                for(int n = lamp_height/3; n < lamp_height; n++) {
                    world->AddBlock(x_+i,road_pos_y+n,z_+j, 205, 206, 209);
                    if(n == lamp_height-1) {
                        // Create actual lamp
                        Vector3 lpos;
                        if(type == 0) {
                            lpos.x_ = x_;
                            lpos.y_ = road_pos_y+n;
                            lpos.z_ = z_-12;
                            for(int q = 0; q < 5; q++) {
                                world->AddBlock(x_,road_pos_y+n,z_-q, 205, 206, 209);
                            }
                            for(int q = -1; q < 2; q++) {
                                for(int p = 0; p < 5; p++) {
                                    for(int m = 0; m < 2; m++) {
                                        world->AddBlock(x_+q,road_pos_y+n-m,z_-4-p, 140, 150, 160);
                                    }
                                }
                            }
                       } else if(type == 1) {
                           lpos.x_ = x_;
                           lpos.y_ = road_pos_y+n;
                           lpos.z_ = z_+12;
                           for(int q = 0; q < 5; q++) {
                               world->AddBlock(x_,road_pos_y+n,z_+q+1, 205, 206, 209);
                           }
                           for(int q = -1; q < 2; q++) {
                               for(int p = 0; p < 5; p++) {
                                   for(int m = 0; m < 2; m++) {
                                       world->AddBlock(x_+q,road_pos_y+n-m,z_+5+p, 140, 150, 160);
                                   }
                               }
                           }
                       } else if(type == 2) {
                           lpos.x_ = x_-12;
                           lpos.y_ = road_pos_y+n;
                           lpos.z_ = z_;
                           for(int q = 0; q < 5; q++) {
                               world->AddBlock(x_-q,road_pos_y+n,z_, 205, 206, 209);
                           }
                           for(int q = -1; q < 2; q++) {
                               for(int p = 0; p < 5; p++) {
                                   for(int m = 0; m < 2; m++) {
                                       world->AddBlock(x_-4-p,road_pos_y+n-m,z_+q, 140, 150, 160);
                                   }
                               }
                           }
                        } else if(type == 3) {
                            lpos.x_ = x_+12;
                            lpos.y_ = road_pos_y+n;
                            lpos.z_ = z_;
                            for(int q = 0; q < 5; q++) {
                                world->AddBlock(x_+q,road_pos_y+n,z_, 205, 206, 209);
                            }
                            for(int q = -1; q < 2; q++) {
                                for(int p = 0; p < 5; p++) {
                                    for(int m = 0; m < 2; m++) {
                                        world->AddBlock(x_+4+p,road_pos_y+n-m,z_+q, 140, 150, 160);
                                    }
                                }
                            }
                        }


                        if(Random(10) < 4) {
                            Node* lightNode = scene->CreateChild();
                            //lightNode->SetPosition(Vector3(x_+1,road_pos_y+n,z_-6));
                            lightNode->SetPosition(lpos);
                            lightNode->SetDirection(Vector3::FORWARD);
                            lightNode->Yaw(90);
                            lightNode->Pitch(90);
                            Light* light=lightNode->CreateComponent<Light>();
                            light->SetRange(40);
                            light->SetFov(80);
                            light->SetLightType(LIGHT_SPOT);
                            light->SetBrightness(1.9);
                            light->SetColor(Color(0.7,0.9,0.5,1));
                            light->SetCastShadows(false);
                        }
                    }
                }
            }
        }
    }
}

void Map::Level1()
{

    // Ground
    std::vector<Vector3> buildings;
    std::vector<Vector3> trees;

    // Create some roads
    int size = 200;
    for(int x = 0; x < map_size_x; x++) {
        for(int z = 0; z < map_size_z; z++) {
            if(x == 0 || z == 0|| x == map_size_x-1 || z == map_size_z-1) {
                // Fence around city
                for(int yy = 0; yy < 30; yy++) {
                    if(z == map_size_z-1 && x == map_size_x-1) {
                        for(int z_ = 0; z_ < road_size_z; z_++) {
                            world->AddBlock(x+road_size_x, yy, z+z_, 150, 150, 150);
                            world->AddBlock(x+z_, yy, z+road_size_z, 150, 150, 150);
                        }
                    } else if(z == 0 && x == 0) {
                        for(int z_ = 0; z_ < road_size_z; z_++) {
                            world->AddBlock(x-road_size_x, yy, z-z_, 150, 150, 150);
                            world->AddBlock(x-z_, yy, z-road_size_z, 150, 150, 150);
                        }
                    } else if(z == map_size_z-1) {
                        world->AddBlock(x, yy, z+road_size_z, 150, 150, 150);
                        for(int z_ = 0; z_ < road_size_z; z_++) {
                            world->AddBlock(0, yy, z+z_, 150, 150, 150);
                        }
                    } else if(x == map_size_x-1) {
                        world->AddBlock(x+road_size_x, yy, z, 150, 150, 150);
                        for(int x_ = 0; x_ < road_size_x; x_++) {
                            world->AddBlock(x+x_, yy, 0, 150, 150, 150);
                        }
                    } else {
                        world->AddBlock(x, yy, z, 150, 150, 150);
                    }
                }
            }
           // if(x % road_size_x == 0 && z % road_size_z == 0) {
           //     size = 200+Random(200);
           //     size = 100 * (size/100);
           //     cout << "SIZE: " << size << endl;
           // }
            if(x % size == 0 || x == 0 || x == map_size_x - 1) {
                for(int xx = x; xx < x+road_size_x; xx++) {
                    for(int zz = z; zz < z+road_size_z; zz++) {
                     //   if(xx-x == road_size_x/2 && zz % 11 < 5 && z+zz % size != 0) {
                     //       world->AddBlock(xx,road_pos_y,zz, 250, 250, 250);
                     //   } else {
                            world->AddBlock(xx,road_pos_y,zz, 103, 104, 108);
                      //  }
                            layout[x][z] = 1;
                    }
                }
            }
            if(z % size == 0 || z == 0 || z == map_size_z - 1) {
                for(int xx = x; xx < x+road_size_x; xx++) {
                    for(int zz = z; zz < z+road_size_z; zz++) {
                    //    if(zz-z == road_size_z/2 && xx % 11 < 5 && x+xx % size != 0) {
                    //        world->AddBlock(xx,road_pos_y,zz, 250, 250, 250);
                    //    } else {
                            world->AddBlock(xx,road_pos_y,zz, 103, 104, 108);
                    //    }
                            layout[x][z] = 1;
                    }
                }
            }
        }
    }

    // Now fill squares between streets with either grass or concrete
    int light_offset = 80;
    int type = 0; // 0 grass, 1 concrete TBD: Create static constants for these.
    int r,g,b;
    for(int x = 0; x < map_size_x; x++) {
        for(int z = 0; z < map_size_z; z++) {
            if(world->Exists(x,road_pos_y,z)) {
                continue;
            }
            if(Random(10) > 5) {
                // Grass
                type = 0;
                r = 145;
                g = 205;
                b = 74;
            } else {
                // Concrete
                type = 1;
                r = 151;
                g = 154;
                b = 158;
            }
            int max_x = 0;
            int max_z = 0;
            int xx = x;
            int zz = z;
            while(!world->Exists(xx++, road_pos_y, z)) {
                max_x++;
            }
            while(!world->Exists(x, road_pos_y, zz++)) {
                max_z++;
            }
            for(int x_ = x; x_ <= x + max_x; x_++) {
                for(int z_ = z; z_ <= z + max_z; z_++) {
                    // Add traffic lights
                    if(x_ == x+1 && z_ == z+1) {
                        // First direction
                        GenerateTrafficLight(Vector3(x_, road_pos_y+1, z_), 3);
                        // Crossroad
                        if(Random(2) < 1) {
                            for(int xx = x_; xx > x_-road_size_x; xx--) {
                                for(int zz = z_; zz < z_+road_size_z/2; zz++) {
                                    if(xx % 7 < 4) {
                                        world->AddBlock(xx,road_pos_y,zz, 250, 250, 250);
                                    }
                                }
                            }
                        }
                    } else if(x_ == x+max_x-1 && z_ == z+1) {
                        // Crossroad
                        if(Random(2) < 1) {
                            for(int xx = x_; xx > x_-road_size_x/2; xx--) {
                                for(int zz = z_; zz > z_-road_size_z; zz--) {
                                    if(zz % 7 < 4) {
                                        world->AddBlock(xx,road_pos_y,zz, 250, 250, 250);
                                    }
                                }
                            }
                        }
                        GenerateTrafficLight(Vector3(x_, road_pos_y+1, z_), 2);
                    } else if(x_ == x+max_x-1 && z_ == z + max_z-1) {
                        // Crossroad
                        if(Random(2) < 1) {
                            for(int xx = x_; xx < x_+road_size_x; xx++) {
                                for(int zz = z_; zz > z_-road_size_z/2; zz--) {
                                    if(xx % 7 < 4) {
                                        world->AddBlock(xx,road_pos_y,zz, 250, 250, 250);
                                    }
                                }
                            }
                        }
                        GenerateTrafficLight(Vector3(x_, road_pos_y+1, z_), 1);
                    } else if(x_ == x+1 && z_ == z + max_z-1) {
                        if(Random(2) < 1) {
                            for(int xx = x_; xx < x_+road_size_x/2; xx++) {
                                for(int zz = z_; zz < z_+road_size_z; zz++) {
                                    if(zz % 7 < 4) {
                                        world->AddBlock(xx,road_pos_y,zz, 250, 250, 250);
                                    }
                                }
                            }
                        }
                        GenerateTrafficLight(Vector3(x_, road_pos_y+1, z_), 0);
                    }

                    // Add lights
                    if(x_ % light_offset == 0 && x_ != x && x_ != x+max_x && z_-z == side_walk_size-1) {
                        // First direction
                        GenerateLight(Vector3(x_, road_pos_y+1, z_), 0);
                    } else if(x_ % light_offset == 0 && x_ != x && x_ != x+max_x && z_-z == max_z - side_walk_size) {
                        // Second direction 
                        GenerateLight(Vector3(x_, road_pos_y+1, z_), 1);
                    }
                    if(z_ % light_offset == 0 && z_ != z && z_ != z+max_z && x_-x == side_walk_size-1) {
                        // Third direction
                        GenerateLight(Vector3(x_, road_pos_y+1, z_), 2);
                    } else if(z_ % light_offset == 0 && z_ != z && z_ != z+max_z && x_-x == max_x - side_walk_size) {
                        // Forth direction
                        GenerateLight(Vector3(x_, road_pos_y+1, z_), 3);
                    }

                    // Add sidewalks
                    if(z_ < z + side_walk_size || z_ >= z + max_z - side_walk_size) {
                        world->AddBlock(x_,road_pos_y+1,z_, 232, 235, 239);
                    } else if(x_ < x + side_walk_size || x_ >= x + max_x - side_walk_size) {
                        world->AddBlock(x_,road_pos_y+1,z_, 232, 235, 239);
                    }
                    world->AddBlock(x_,road_pos_y,z_, r, g, b);

                }
            }
            if(type == 1) {
                GenerateBuildings(Vector3(x+side_walk_size, road_pos_y+1, z+side_walk_size), 
                                  Vector3(x+max_x-side_walk_size, road_pos_y+1, z+max_z-side_walk_size)
                                 );
            } else if(type == 0)  {
                GeneratePark(Vector3(x+side_walk_size, road_pos_y+1, z+side_walk_size), 
                             Vector3(x+max_x-side_walk_size, road_pos_y+1, z+max_z-side_walk_size)
                            );
            }
        }
    }


    Node* zoneNode = scene->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 3000.0f));
    zone->SetAmbientColor(Color(0.3f, 0.3f, 0.2f));
    zone->SetFogColor(Color(0.087f, 0.087f, 0.087f));
   // zone->SetFogStart(200.0f);
   // zone->SetFogEnd(1600.0f);

//    Node* skyNode=scene->CreateChild("Sky");
//    skyNode->SetScale(10.0f); // The scale actually does not matter
//    Skybox* skybox=skyNode->CreateComponent<Skybox>();
//    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
//    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox1.xml"));

    // Create a red directional light (sun)
    Node* lightNode=scene->CreateChild();
    lightNode->SetPosition(Vector3(100,500,500));
    lightNode->SetDirection(Vector3::FORWARD);
    lightNode->Yaw(40);     // horizontal
    lightNode->Pitch(70);   // vertical
    Light* light=lightNode->CreateComponent<Light>();
    //light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.1);
    light->SetColor(Color(0.8,0.8,0.8, 0.5));
    light->SetCastShadows(true);
    light->SetShadowResolution(1.0);
    light->SetShadowIntensity(0.1);
    //light->SetTemperature(800.0f);

    light->SetShadowBias(BiasParameters(0.0015f, 1.5f));
    // Set cascade splits at 10, 50 and 200 world units, fade shadows out at 80% of maximum shadow distance
     //light->SetShadowCascade(CascadePrameters(10.0f, 200.0f, 800.0f, 0.0f, 0.8f));
    

    // Water bottom
 //   Node *bottom = scene->CreateChild("Water");
 //   bottom->SetScale(Vector3(5000.0f, 1.0f, 5000.0f));
 //   bottom->SetPosition(Vector3(0.0f, -2, 0.0f));
 //   StaticModel* bottom_model = bottom->CreateComponent<StaticModel>();
 //   bottom_model->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
 //   bottom_model->SetMaterial(cache->GetResource<Material>("Materials/bottom.xml"));
 //   
 //   //
 //   // Water
 //   //
 //   Node *waterNode_ = scene->CreateChild("Water");
 //   waterNode_->SetScale(Vector3(3000.0f, 1.0f, 3000.0f));
 //   waterNode_->SetPosition(Vector3(0.0f, 0, 0.0f));
 //   StaticModel* water = waterNode_->CreateComponent<StaticModel>();
 //   water->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
 //   water->SetMaterial(cache->GetResource<Material>("Materials/Water.xml"));
 //   // Set a different viewmask on the water plane to be able to hide it from the reflection camera
 //   water->SetViewMask(0x80000000);

 //      // Create a mathematical plane to represent the water in calculations
 //   Plane waterPlane_ = Plane(waterNode_->GetWorldRotation() * Vector3(0.0f, 1.5f, 0.0f), waterNode_->GetWorldPosition());
 //   // Create a downward biased plane for reflection view clipping. Biasing is necessary to avoid too aggressive clipping
 //   Plane waterClipPlane_ = Plane(waterNode_->GetWorldRotation() * Vector3(0.0f, 1.5f, 0.0f), waterNode_->GetWorldPosition() -
 //       Vector3(0.0f, 0.1f, 0.0f));

 //   // Create camera for water reflection
 //   // It will have the same farclip and position as the main viewport camera, but uses a reflection plane to modify
 //   // its position when rendering
 //   Node *cameraNode_ = GetSubsystem<Game>()->cameraNode_;
 //   Graphics *graphics_ = GetSubsystem<Game>()->graphics_;

 //   Node *reflectionCameraNode_ = cameraNode_->CreateChild();
 //   Camera* reflectionCamera = reflectionCameraNode_->CreateComponent<Camera>();
 //   reflectionCamera->SetFarClip(800.0);
 //   reflectionCamera->SetViewMask(0x7fffffff); // Hide objects with only bit 31 in the viewmask (the water plane)
 //   reflectionCamera->SetAutoAspectRatio(false);
 //   reflectionCamera->SetUseReflection(true);
 //   reflectionCamera->SetReflectionPlane(waterPlane_);
 //   reflectionCamera->SetUseClipping(true); // Enable clipping of geometry behind water plane
 //   reflectionCamera->SetClipPlane(waterClipPlane_);
 //   // The water reflection texture is rectangular. Set reflection camera aspect ratio to match
 //   reflectionCamera->SetAspectRatio((float)graphics_->GetWidth() / (float)graphics_->GetHeight());
 //   // View override flags could be used to optimize reflection rendering. For example disable shadows
 //   //reflectionCamera->SetViewOverrideFlags(VO_DISABLE_SHADOWS);

 //   // Create a texture and setup viewport for water reflection. Assign the reflection texture to the diffuse
 //   // texture unit of the water material
 //   int texSize = 1024;
 //   SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
 //   renderTexture->SetSize(texSize, texSize, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
 //   renderTexture->SetFilterMode(FILTER_BILINEAR);
 //   RenderSurface* surface = renderTexture->GetRenderSurface();
 //   SharedPtr<Viewport> rttViewport(new Viewport(context_, scene, reflectionCamera));
 //   surface->SetViewport(0, rttViewport);
 //   Material* waterMat = cache->GetResource<Material>("Materials/Water.xml");
 //   waterMat->SetTexture(TU_DIFFUSE, renderTexture);
}

