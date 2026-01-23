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
    uint32_t SpawnJumpBoss(const SquareCore::Vec2& position = SquareCore::Vec2(-6000.0f, 50.0f));

private:
    void AlterChargeEnemy(uint32_t enemy_id);
    void AlterJumpEnemy(uint32_t enemy_id);

private:
    uint32_t player = 0;
    
    std::vector<uint32_t> enemies;
    
    std::vector<SquareCore::Vec2> enemy_collider_vertices={
        SquareCore::Vec2(-41.25f, -41.25f),
        SquareCore::Vec2(0.0f, 41.25f),
        SquareCore::Vec2(41.25f, -41.25f)
    };
    std::vector<SquareCore::Vec2> boss_collider_vertices={
        SquareCore::Vec2(-412.5f, -412.5f),
        SquareCore::Vec2(0.0f, 412.5f),
        SquareCore::Vec2(412.5f, -412.5f)
    };

    uint32_t jump_boss = 0;
    bool jump_boss_active = false;
};