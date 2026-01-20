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

    uint32_t jump_boss = 0;
};