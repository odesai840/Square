#pragma once

#include "Script.h"
#include "Properties.h"

class EnemyManager : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    uint32_t player = 0;
    
    std::vector<uint32_t> enemies;
    unsigned int jump_enemy_count = 0;
    unsigned int charge_enemy_count = 1;
    
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
};