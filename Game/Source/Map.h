#pragma once

#include "EnemyManager.h"
#include "Player.h"
#include "Script.h"

class UserInterface;

class Map : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void LoadMap(int level, SquareCore::Vec2 player_position);
    void SetEnemyManager(EnemyManager* enemy_manager) { this->enemy_manager = enemy_manager; }
    void SetPlayer(Player* player) { this->player = player; }
    void SetUserInterface(UserInterface* ui) { this->ui = ui; }

private:
    uint32_t main_menu_music = 0;
    uint32_t level_1_music = 0;
    uint32_t level_2_music = 0;
    uint32_t level_3_music = 0;
    EnemyManager* enemy_manager = nullptr;
    Player* player = nullptr;
    UserInterface* ui = nullptr;

    int current_map = 0;
    std::vector<uint32_t> wormholes;
    std::vector<uint32_t> wormholes_negative;
    float wormhole_rotation = 0.0f;
    float wormhole_speed = 55.0f;

    std::vector<uint32_t> ability_icons;
    float ability_icon_rotation = 0.0f;
    float ability_icon_speed = 15.0f;
    
    uint32_t ball_entity = 0;
    uint32_t crushed_enemy = 0;
    
    uint32_t level_1_boss_door = 0;
};
