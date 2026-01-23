#include "Map.h"

#include "PlayerKeybinds.h"

void Map::OnStart()
{
    main_menu_music = AddAudioClip("Resources/Audio/29th.wav");
    level_1_music = AddAudioClip("Resources/Audio/03.mp3");
    level_2_music = AddAudioClip("Resources/Audio/03.mp3");
    level_3_music = AddAudioClip("Resources/Audio/03.mp3");
    SetAudioVolume(main_menu_music, 0.1f);
    SetAudioVolume(level_1_music, 0.1f);
    SetAudioVolume(level_2_music, 0.1f);
    SetAudioVolume(level_3_music, 0.1f);
    SetAudioLooping(main_menu_music, true);
    SetAudioLooping(level_1_music, true);
    SetAudioLooping(level_2_music, true);
    SetAudioLooping(level_3_music, true);
    SetAudioMasterVolume(0.0f);
    PlayAudioClip(main_menu_music);
    
    SetCameraZoom(0.85f);
    //SetCameraZoom(0.1f);
    //SetCameraBounds(-14000.0f, -400.0f, 6000.0f, 10000.0f); // Level 1 bounds
    SetCameraBounds(-10000.0f, -6000.0f, 6000.0f, 10000.0f);
    EnableCameraBounds(true);
    SetGravity(-1500.0f);
}

void Map::OnUpdate(float deltaTime)
{
    wormhole_rotation += deltaTime * wormhole_speed;
    for (int i = 0 ; i < wormholes.size(); i++)
    {
        SetRotation(wormholes[i], wormhole_rotation * (1.0f + (i*0.4f)));
    }
    for (int i = 0 ; i < wormholes_negative.size(); i++)
    {
        SetRotation(wormholes_negative[i], -wormhole_rotation * (1.0f + (i*0.1f)));
    }

    if (GetKeyPressed(debug_hot_reload))
    {
        uint32_t player_id = GetFirstEntityWithTag("Player");
        SquareCore::Vec2 player_pos = GetPosition(player_id);
        LoadMap(current_map, {player_pos.x, player_pos.y});
    }
}

void Map::LoadMap(int level, SquareCore::Vec2 player_position)
{
    LoadScene(level_path);
    if (enemy_manager) enemy_manager->LoadEnemies();
    if (player) player->TeleportPlayer({player_position.x, player_position.y});

    wormholes = GetAllEntitiesWithTag("Wormhole");
    wormholes_negative = GetAllEntitiesWithTag("WormholeNegative");
    for (uint32_t wormhole : wormholes)
    {
        SetColliderType(wormhole, SquareCore::ColliderType::NONE);
    }
    for (uint32_t wormhole : wormholes_negative)
    {
        SetColliderType(wormhole, SquareCore::ColliderType::NONE);
    }
    
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
