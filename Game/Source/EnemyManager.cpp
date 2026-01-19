#include "EnemyManager.h"

void EnemyManager::OnStart()
{
    for (unsigned int i = 0; i < enemy_count; i++)
    {
        Enemy enemy = {AddEntity("Resources/Sprites/triangle.png", 0.0f + (100.0f * i), 0.0f, 0.0f, 0.05f, 0.05f, true), Health()};
        enemies.insert(enemies.end(), enemy);
        SetEntityColor(enemies[i].id, SquareCore::RGBA(120, 0, 0, 255));
        AddTagToEntity(enemies[i].id, "Enemy");
        SetDrag(enemies[i].id, 5.0f);
        SetEntityPersistent(enemies[i].id, true);
        AddPropertyToEntity(enemies[i].id, new Health());
    }
}

void EnemyManager::OnUpdate(float deltaTime)
{
    
}

