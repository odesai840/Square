#pragma once

#include "EnemyManager.h"
#include "Script.h"

class Map : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void LoadMap();
    void SetEnemyManager(EnemyManager* enemy_manager) { this->enemy_manager = enemy_manager; }

private:
    uint32_t ground = 0;
    uint32_t music = 0;
    uint32_t dialogTestTrigger = 0;
    EnemyManager* enemy_manager = nullptr;
};