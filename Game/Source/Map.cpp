#include "Map.h"

#include "PlayerKeybinds.h"
#include "UserInterface.h"

void Map::OnStart()
{
    main_menu_music = AddAudioClip("Resources/Audio/29th.wav");
    level_1_music = AddAudioClip("Resources/Audio/12.mp3");
    level_2_music = AddAudioClip("Resources/Audio/03.mp3");
    level_3_music = AddAudioClip("Resources/Audio/11.mp3");
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

    ability_icon_rotation += deltaTime * ability_icon_speed;
    for (uint32_t icon : ability_icons)
    {
        if (!EntityExists(icon)) continue;
        SetRotation(icon, ability_icon_rotation);
    }

    if (GetKeyPressed(debug_hot_reload))
    {
        uint32_t player_id = GetFirstEntityWithTag("Player");
        SquareCore::Vec2 player_pos = GetPosition(player_id);
        LoadMap(current_map, {player_pos.x, player_pos.y});
    }
    
    if (EntityExists(ball_entity))
    {
        auto collisions = GetEntityCollisions(ball_entity);
        for (auto collision : collisions)
        {
            if (!EntityExists(collision.first)) continue;
            
            if (EntityHasTag(collision.first, "CrushedEnemyShield"))
            {
                RemoveEntity(crushed_enemy);
                RemoveEntity(collision.first);
                continue;
            }
            
            if (EntityHasTag(collision.first, "Ground"))
            {
                uint32_t ball_rubble = GetFirstEntityWithTag("BallRubble");
                
                SetEntityColor(ball_rubble, SquareCore::RGBA(255, 255, 255, 255));
                
                SetRotation(level_1_boss_door, 70.0f);
                SetPosition(level_1_boss_door, -7900.0f, -100.0f);
                
                RemoveEntity(ball_entity);
                
                enemy_manager->SpawnJumpBoss({-11000.0f, 500.0f});
            }
        }
    }
}

void Map::LoadMap(int level, SquareCore::Vec2 player_position)
{
    current_map = level;
    std::string scene_path;
    switch (level)
    {
    case 0:
        scene_path = "Resources/Scenes/main_menu.square";
        break;
    case 1:
        scene_path = "Resources/Scenes/level1.square";
        break;
    case 2:
        scene_path = "Resources/Scenes/level2.square";
        break;
    case 3:
        scene_path = "Resources/Scenes/level3.square";
        break;
    default:
        scene_path = "Resources/Scenes/main_menu.square";
        break;
    }
    
    LoadScene(scene_path);
    if (enemy_manager) enemy_manager->LoadEnemies();
    if (player_script) player_script->TeleportPlayer({player_position.x, player_position.y});

    ability_icons = GetAllEntitiesWithTag("Ability");
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

    if (uint32_t boss_2_exit = GetFirstEntityWithTag("Boss2Exit"))
    {
        if (player_script->GetPlayerData().second_boss_dead)
        {
            RemoveEntity(boss_2_exit);
        }
    }

    player_script->HealMaxHealth();
    
    ball_entity = GetFirstEntityWithTag("Ball");
    crushed_enemy = GetFirstEntityWithTag("CrushedEnemy");
    level_1_boss_door = GetFirstEntityWithTag("Level1BossDoor");
    
    uint32_t ability = GetFirstEntityWithTag("GainDoubleDash");
    if (ability && player_script->GetPlayerData().has_double_dash)
        RemoveEntity(ability);
    ability = GetFirstEntityWithTag("GainDoubleJump") ;
    if (ability && player_script->GetPlayerData().has_double_jump)
        RemoveEntity(ability);
    ability = GetFirstEntityWithTag("GainProjectile");
    if (ability  && player_script->GetPlayerData().has_projectile)
        RemoveEntity(ability);
    ability = GetFirstEntityWithTag("GainFasterSlash");
    if (ability  && player_script->GetPlayerData().has_faster_slash)
        RemoveEntity(ability);
    
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
            SetCameraBounds(-14000.0f, -400.0f, 6000.0f, 10000.0f);
            if (ui) ui->AreaTitle("The Cage", "Now Playing:\nThe Cage\nCaleb Kronstad and Ohm Desai");
            break;
        }
    case 2:
        {
            StopAudioClip(level_1_music);
            StopAudioClip(main_menu_music);
            StopAudioClip(level_3_music);
            PlayAudioClip(level_2_music);
            SetCameraBounds(-10000.0f, -400.0f, 6000.0f, 10000.0f);
            if (ui) ui->AreaTitle("The Slums", "Now Playing:\nThe Slums\nCaleb Kronstad");
            //player_script->TeleportPlayer(SquareCore::Vec2(-5500.0, -4550.0f));
            break;
        }
    case 3:
        {
            StopAudioClip(level_1_music);
            StopAudioClip(level_2_music);
            StopAudioClip(main_menu_music);
            PlayAudioClip(level_3_music);
            if (ui) ui->AreaTitle("The Palace", "Now Playing:\nThe Palace\nCaleb Kronstad");
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
