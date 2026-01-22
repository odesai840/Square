#pragma once

#include "EnemyManager.h"
#include "Player.h"
#include "Script.h"

class Map : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void LoadMap(int level, SquareCore::Vec2 player_position);
    void SetEnemyManager(EnemyManager* enemy_manager) { this->enemy_manager = enemy_manager; }
    void SetPlayer(Player* player) { this->player = player; }

private:
    uint32_t main_menu_music = 0;
    uint32_t level_1_music = 0;
    uint32_t level_2_music = 0;
    uint32_t level_3_music = 0;
    uint32_t dialogTestTrigger = 0;
    EnemyManager* enemy_manager = nullptr;
    Player* player = nullptr;

    int current_map = 0;
};