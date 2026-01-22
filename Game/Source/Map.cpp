#include "Map.h"

#include "PlayerKeybinds.h"

void Map::OnStart()
{
    main_menu_music = AddAudioClip("Resources/Audio/03.mp3");
    level_1_music = AddAudioClip("Resources/Audio/03.mp3");
    level_2_music = AddAudioClip("Resources/Audio/03.mp3");
    level_3_music = AddAudioClip("Resources/Audio/03.mp3");
    SetAudioVolume(main_menu_music, 1.0f);
    SetAudioVolume(level_1_music, 0.05f);
    SetAudioVolume(level_2_music, 0.05f);
    SetAudioVolume(level_3_music, 0.05f);
    SetAudioLooping(main_menu_music, true);
    SetAudioLooping(level_1_music, true);
    SetAudioLooping(level_2_music, true);
    SetAudioLooping(level_3_music, true);
    
    PlayAudioClip(main_menu_music);
    
    /*dialogTestTrigger = AddSpritelessEntity(10, 10, SquareCore::RGBA(0,0,0,0), 200, -250);
    SetColliderType(dialogTestTrigger, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(dialogTestTrigger, "DialogTrigger");
    SetEntityPersistent(dialogTestTrigger, true);*/
    
    SetCameraZoom(0.85f);
    SetCameraBounds(-10000.0f, -400.0f, 6000.0f, 10000.0f);
    EnableCameraBounds(true);
    SetGravity(-1500.0f);
}

void Map::OnUpdate(float deltaTime)
{
    if (GetKeyPressed(debug_hot_reload))
        LoadMap(current_map, {-100.0f, 200.0f});
}

void Map::LoadMap(int level, SquareCore::Vec2 player_position)
{
    LoadScene(level_path);
    if (enemy_manager) enemy_manager->LoadEnemies();
    if (player) player->TeleportPlayer({player_position.x, player_position.y});
    
    switch (level)
    {
    case 0:
        {
            StopAudioClip(level_1_music);
            StopAudioClip(level_2_music);
            StopAudioClip(level_3_music);
            PlayAudioClip(main_menu_music);
            break;
        }
    case 1:
        {
            StopAudioClip(main_menu_music);
            StopAudioClip(level_2_music);
            StopAudioClip(level_3_music);
            PlayAudioClip(level_1_music);
            break;
        }
    case 2:
        {
            StopAudioClip(level_1_music);
            StopAudioClip(main_menu_music);
            StopAudioClip(level_3_music);
            PlayAudioClip(level_2_music);
            break;
        }
    case 3:
        {
            StopAudioClip(level_1_music);
            StopAudioClip(level_2_music);
            StopAudioClip(main_menu_music);
            PlayAudioClip(level_3_music);
            break;
        }
    default:
        {
            StopAudioClip(level_1_music);
            StopAudioClip(level_2_music);
            StopAudioClip(level_3_music);
            StopAudioClip(main_menu_music);
            break;
        }
    }
    
}
