#include "main.h"
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace Urho3D;

Game::Game(Context * context): Application(context) 
{
    framecount = 0;
    time = 0;
    context_->RegisterSubsystem(this);
}

Game::~Game() 
{
    delete world;
}

void Game::Setup() 
{
    // See http://urho3d.github.io/documentation/1.5/_main_loop.html
    // engineParameters_["WindowIcon"] = "";
    engineParameters_["LowQualityShadows"] = true;
    engineParameters_["TrippleBuffer"] = true;
    engineParameters_["FullScreen"] = false;
    engineParameters_["HighDPI"] = false;
    engineParameters_["WindowTitle"] = "The Game";
    engineParameters_["WindowWidth"] = 800;
    engineParameters_["WindowHeight"] = 600;
    engineParameters_["WindowResizable"] = true;
    engineParameters_["VSync"] = true;
    engineParameters_["Multisample"] = 2;
    engineParameters_["ResourcePrefixPaths"] = "./Materials/;/Users/nergal/Downloads/Urho3D-1.7-macOS-64bit-STATIC/share/Urho3D/Resources/";

}

void Game::Start()
{


    for(int i = 0; i < 100000; i++) {
        indexData.Push(i);
    }

    context_->RegisterFactory<Chunk>();


    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Let's use the default style that comes with Urho3D.
    GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    // Let's create some text to display.
    text_ = new Text(context_);
    text_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),12);
    text_->SetColor(Color(1.0, 1.0, 1.0));
    text_->SetHorizontalAlignment(HA_RIGHT);
    text_->SetVerticalAlignment(VA_TOP);
    GetSubsystem<UI>()->GetRoot()->AddChild(text_);


    // Let's setup a scene to render.
    scene_ = new Scene(context_);
    graphics_ = new Graphics(context_);
    time = new Time(context_);
    scene_->CreateComponent<Octree>();
    // scene_->CreateComponent<PhysicsWorld>();
    if(debug) {
        scene_->CreateComponent<DebugRenderer>();
    }

    //scene_->GetComponent<PhysicsWorld>()->SetGravity(Vector3(0, -39.82, 0));


    float start_time = GetSubsystem<Game>()->time->GetElapsedTime();
    world = new World(context_);

    cameraNode_= scene_->CreateChild("Camera");
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    //  camera->SetUseClipping(true);
    camera->SetFarClip(500);
    //   camera->SetFov(45);
    //  camera->SetOrthographic(true);
    //  camera->SetOrthoSize(1);

    // camera->SetFillMode(FILL_WIREFRAME);

    GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

    SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(Game,HandleUpdate));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Game, HandlePostUpdate));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Game, HandleKeyDown));

    if(debug) {
        XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
        DebugHud* debugHud=engine_->CreateDebugHud();
        debugHud->SetDefaultStyle(style);
        debugHud->SetMode(DEBUGHUD_SHOW_STATS); // DBEUGHUD_SHOW_ALL)
    }

    float end_time = GetSubsystem<Game>()->time->GetElapsedTime();

    map = new Map(context_);
    map->Level1();


    player = new Char(context_);

    for(int i = 0; i < 100; i++) {
        enemies.push_back(new Enemy(context_, Vector3(10+Random(500), 2, 10+Random(500))));
    }
//
//    for(int i = 0; i < 10; i++) {
//        souls.push_back(new Soul(context_, Vector3(20+Random(100), 30, 20+Random(100))));
//    }


    std::cout << "INIT TIME: " << end_time - start_time << std::endl;
}

void Game::Stop()
{
}

void Game::HandleKeyDown(StringHash eventType,VariantMap& eventData)
{
    using namespace KeyDown;
    int key=eventData[P_KEY].GetInt();

    if(key == KEY_ESCAPE) {
        engine_->Exit();
    } else if(key == KEY_TAB) {
        GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
        GetSubsystem<Input>()->SetMouseGrabbed(!GetSubsystem<Input>()->IsMouseGrabbed());
    }
}

void Game::HandleClosePressed(StringHash eventType,VariantMap& eventData)
{
    engine_->Exit();
}


/**
 * Your non-rendering logic should be handled here.
 * This could be moving objects, checking collisions and reaction, etc.
 */
void Game::HandleUpdate(StringHash eventType,VariantMap& eventData)
{
    float timeStep=eventData[Update::P_TIMESTEP].GetFloat();
    framecount++;
    time_+=timeStep;

    // World update
    world->Update(time_, timeStep);
    player->Update(time_, timeStep);

    for(int i = 0; i < souls.size(); i++) {
        souls[i]->Update(time_, timeStep);
    }
    for(int i = 0; i < enemies.size(); i++) {
        enemies[i]->Update(time_, timeStep);
    }
    // map->Update(time_, timeStep);

    if(time_ > 1)
    {
        std::string str;
        //  str.append("Keys: tab = toggle mouse, AWSD = move camera, Shift = fast mode, Esc = quit.\n");
        {
            std::ostringstream ss;
            ss << " "<< framecount;
            std::string s(ss.str());
            str.append(s.substr(0,6));
        }
        str.append(" frames in ");
        {
            std::ostringstream ss;
            ss<<time_;
            std::string s(ss.str());
            str.append(s.substr(0,6));
        }
        str.append(" seconds = ");
        {
            std::ostringstream ss;
            ss<<(float)framecount/time_;
            std::string s(ss.str());
            str.append(s.substr(0,6));
        }
        str.append(" fps\n");
        {
            std::ostringstream ss;
            ss << " Triangle Count: " << world->GetTriangles() << "\n";
            ss << " Blocks: " << world->GetBlocks() << "\n";
            ss << " Chunks: " << world->GetChunks() << "\n";
            ss << " Dirty Chunks: " << world->GetDirtyChunks();
            std::string s(ss.str());
            str.append(s.substr(0,70));
        }
        String s(str.c_str(),str.size());
        text_->SetText(s);
        //URHO3D_LOGINFO(s);     // this show how to put stuff into the log
        framecount=0;
        time_ = 0;
    }

}

void Game::HandlePostUpdate(StringHash eventType, VariantMap & eventData)
{

}

URHO3D_DEFINE_APPLICATION_MAIN(Game)
