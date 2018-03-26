#ifndef __MAIN_H__
#define __MAIN_H__

#include <string>
#include <sstream>
#include <iostream>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/Texture2D.h>

#include "world.h"
#include "vox.h"
#include "constants.h"
#include "char.h"
#include "map.h"
#include "enemy.h"
#include "soul.h"

const static bool debug = true;

using namespace Urho3D;

class Game : public Application 
{
public:
    Game(Context*);
    ~Game();
    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    void HandleClosePressed(StringHash, VariantMap&);
    void HandleKeyDown(StringHash, VariantMap&);
    void HandleUpdate(StringHash, VariantMap&);
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);

    World *world;
    Map *map;
    Char *player;

    std::vector<Soul*> souls;
    std::vector<Enemy*> enemies;
    std::vector<Chunk*> body_parts;

    int framecount;
    float time_;

    SharedPtr<Text> text_;
    SharedPtr<Scene> scene_;
    SharedPtr<Graphics> graphics_;
    SharedPtr<Time> time;
    SharedPtr<Node> cameraNode_;

    Vector<unsigned short>indexData;
};
#endif

