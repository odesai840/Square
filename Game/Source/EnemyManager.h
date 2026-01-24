#pragma once

#include <array>

#include "Script.h"
#include "Properties.h"

class Player;

class EnemyManager : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    void LoadEnemies();
    void SetPlayerScript(Player* player_s) { this->player_script = player_s; }
    uint32_t SpawnChargeEnemy(const SquareCore::Vec2& position);
    uint32_t SpawnJumpEnemy(const SquareCore::Vec2& position);
    uint32_t SpawnJumpBoss(const SquareCore::Vec2& position = SquareCore::Vec2(-6000.0f, 50.0f));
    uint32_t SpawnSecondBoss(const SquareCore::Vec2& position = SquareCore::Vec2(0.0f, 0.0f));
    
    bool boss_2_active = false;

private:
    void AlterChargeEnemy(uint32_t enemy_id);
    void AlterJumpEnemy(uint32_t enemy_id);

private:
    uint32_t player = 0;
    Player* player_script = nullptr;
    
    std::vector<uint32_t> enemies;
    
    std::vector<SquareCore::Vec2> enemy_collider_vertices={
        SquareCore::Vec2(-41.25f, -41.25f),
        SquareCore::Vec2(0.0f, 41.25f),
        SquareCore::Vec2(41.25f, -41.25f)
    };
    std::vector<SquareCore::Vec2> boss_collider_vertices={
        SquareCore::Vec2(-275.5f, -275.5f),
        SquareCore::Vec2(0.0f, 275.5f),
        SquareCore::Vec2(275.5f, -275.5f)
    };
    std::vector<SquareCore::Vec2> boss_2_collider_vertices={
        SquareCore::Vec2(-275.5f * 0.25f, -275.5f *0.25f),
        SquareCore::Vec2(0.0f, 275.5f * 0.5f),
        SquareCore::Vec2(275.5f * 0.25f, -275.5f * 0.25f)
    };

    uint32_t jump_boss = 0;
    bool jump_boss_active = false;

    std::array<uint32_t, 3> second_bosses = {0, 0, 0};
    std::vector<int> last_attack_order;
};