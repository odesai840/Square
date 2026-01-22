#include "Map.h"

#include "PlayerKeybinds.h"

void Map::OnStart()
{
    music = AddAudioClip("Resources/Audio/Cathedral KF.mp3");
    SetAudioVolume(music, 0.05f);
    SetAudioLooping(music, true);
    //PlayAudioClip(music);
    
    dialogTestTrigger = AddSpritelessEntity(10, 10, SquareCore::RGBA(0,0,0,0), 200, -250);
    SetColliderType(dialogTestTrigger, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(dialogTestTrigger, "DialogTrigger");
    SetEntityPersistent(dialogTestTrigger, true);
    
    LoadMap();
    
    SetCameraZoom(0.85f);
    SetCameraBounds(-10000.0f, -400.0f, 6000.0f, 10000.0f);
    EnableCameraBounds(true);
    SetGravity(-1500.0f);
}

void Map::OnUpdate(float deltaTime)
{
    if (GetKeyPressed(debug_hot_reload))
        LoadMap();
}

void Map::LoadMap()
{
    LoadScene(level_path);
    if (enemy_manager) enemy_manager->LoadEnemies();
}
