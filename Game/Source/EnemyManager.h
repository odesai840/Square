#pragma once

#include "Script.h"
#include "Properties.h"

struct Enemy
{
    uint32_t id;
    Health health;
};

class EnemyManager : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    std::vector<Enemy> enemies;
    unsigned int enemy_count = 4;
};