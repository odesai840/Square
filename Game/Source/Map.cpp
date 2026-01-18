#include "Map.h"

void Map::OnStart()
{
    ground = AddEntity("Resources/Sprites/square.png", 0.0f, -800.0f, 0.0f, 100.0f, 1.0f, false);
    SetEntityColor(ground, SquareCore::RGBA(0, 50, 0, 255));
    AddTagToEntity(ground, "Ground");
    
    music = AddAudioClip("Resources/Audio/Cathedral KF.mp3");
    SetAudioVolume(music, 0.05f);
    SetAudioLooping(music, true);
    PlayAudioClip(music);
}

void Map::OnUpdate(float deltaTime)
{
}
