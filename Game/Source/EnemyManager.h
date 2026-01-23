#pragma once

#include "Script.h"
#include "Properties.h"

class EnemyManager : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    void LoadEnemies();
    uint32_t SpawnChargeEnemy(const SquareCore::Vec2& position);
    uint32_t SpawnJumpEnemy(const SquareCore::Vec2& position);
    uint32_t SpawnJumpBoss(const SquareCore::Vec2& position);

private:
    uint32_t player = 0;
    
    std::vector<uint32_t> enemies;
    
    std::vector<SquareCore::Vec2> enemy_collider_vertices={
        SquareCore::Vec2(-27.5f, -27.5f),
        SquareCore::Vec2(0.0f, 27.5f),
        SquareCore::Vec2(27.5f, -27.5f)
    };
    std::vector<SquareCore::Vec2> boss_collider_vertices={
        SquareCore::Vec2(-275.0f, -275.0f),
        SquareCore::Vec2(0.0f, 275.0f),
        SquareCore::Vec2(275.0f, -275.0f)
    };

    uint32_t jump_boss = 0;
    bool jump_boss_active = false;
};