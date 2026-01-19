#pragma once

#include "Script.h"

class EnemyManager : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    std::vector<uint32_t> enemies;
    unsigned int enemy_count = 4;
};