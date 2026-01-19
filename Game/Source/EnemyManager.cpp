#include "EnemyManager.h"

void EnemyManager::OnStart()
{
    for (unsigned int i = 0; i < enemy_count; i++)
    {
        enemies.insert(enemies.end(), AddEntity("Resources/Sprites/triangle.png", 0.0f + (100.0f * i), 0.0f, 0.0f, 0.05f, 0.05f, true));
        SetEntityColor(enemies[i], SquareCore::RGBA(120, 0, 0, 255));
        AddTagToEntity(enemies[i], "Enemy");
        SetDrag(enemies[i], 5.0f);
        SetEntityPersistent(enemies[i], true);
        AddPropertyToEntity(enemies[i], new Character(5));
    }
}

void EnemyManager::OnUpdate(float deltaTime)
{
    
}

