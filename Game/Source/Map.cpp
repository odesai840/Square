#include "Map.h"

void Map::OnStart()
{
    //ground = AddEntity("Resources/Sprites/square.png", 0.0f, -800.0f, 0.0f, 5.0f, 1.0f, false);
    //SetEntityColor(ground, SquareCore::RGBA(0, 50, 0, 255));
    //AddTagToEntity(ground, "Ground");
    
    music = AddAudioClip("Resources/Audio/Cathedral KF.mp3");
    SetAudioVolume(music, 0.05f);
    SetAudioLooping(music, true);
    //PlayAudioClip(music);
    
    dialogTestTrigger = AddSpritelessEntity(10, 10, SquareCore::RGBA(0,0,0,0), 200, -250);
    SetColliderType(dialogTestTrigger, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(dialogTestTrigger, "DialogTrigger");
    SetEntityPersistent(dialogTestTrigger, true);
}

void Map::OnUpdate(float deltaTime)
{
}
